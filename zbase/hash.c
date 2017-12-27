
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "hash.h"

#define DEFAULT_HASH_SLOT_SIZE   (128)        /**< 默认的hash 槽长度, 后面每次rehash, 左移4位，满的时候刚好32位 */
#define MAX_HASH_SLOT_SIZE       (2147483648) /**< 最大hash槽的长度 */
#define DEFAULT_HASH_LOAD_FACTOR (2)          /**< 默认rehash 负载因子 */
#define DEFAULT_REHASH_TIME		 (4)		  /**< 默认rehash 的倍数 */

/**
 * hash 节点结构
 */
struct hash_node {
    void *key;              /**< key */
    int klen;               /**< key 长度 */
    unsigned int hval;      /**< 此节点的hash值 */
    void *val;              /**< 值 */
    struct hash_node *next; /**< 槽列表的下一个阶段 */
};
typedef struct hash_node hash_node;

/**
 * 哈希表结构
 */
struct hash_table {
    hash_node **slots;                              /**< hash 槽数组 */
    unsigned int nslot;                             /**< nslot; */
    unsigned int load_factor;                       /**< 负载因子，nelement++ > nslot*load_factor  时进行hash */
    unsigned int nelement;                          /**< 当前的条目个数 */
    int flag;                                       /**< 标记位，可以扩展以便于不同的支持 */
    hash_func_t hash_func;                          /** 哈希函数 */
    hash_free_func_t hdel_func;                     /** 删除节点的时候，私有数据的释放函数 */
    hash_key_compare_func_t hkey_compare_func;      /* 键比较函数，只有相等和不等 */
};

/* 当前的hash表的大小 */
unsigned int hash_element_count(hash_table *htable)
{
    return htable->nelement;
}

/* time33 哈希算法 */
static unsigned int default_time33_hash(void *key, int klen)
{
    unsigned int h = 5381;
    unsigned char *p = (unsigned char *)key;
    while (klen > 0) {
        h += (h << 5) + *p;
        p++;
        klen--;
    }
    return h;
}

/* 简单的内存比较 */
static int default_key_compare(void *key1, int klen1, void *key2, int klen2)
{
    return ((klen1 == klen2) && 0 == memcmp(key1, key2, klen1)) ? 0 : 1;
}

/* 创建哈希表 */
hash_table *hash_create(hash_free_func_t hdel_func, hash_func_t hash_func)
{
    return hash_create2(hdel_func, hash_func, NULL, 0);
}

/* 创建哈希表-扩展 */
hash_table *hash_create2(hash_free_func_t hdel_func, hash_func_t hash_func, 
                           hash_key_compare_func_t hkey_compare_func, int flag)
{
    hash_table *htable = zalloc(sizeof(hash_table));
    htable->nslot = DEFAULT_HASH_SLOT_SIZE;
    htable->slots = zalloc(sizeof(hash_node *) * htable->nslot);

    htable->load_factor = DEFAULT_HASH_LOAD_FACTOR;    
    htable->nelement = 0;
    htable->flag = flag;

    htable->hdel_func = hdel_func;
    
    if (hkey_compare_func) 
        htable->hkey_compare_func = hkey_compare_func;
    else
        htable->hkey_compare_func = default_key_compare;

    if (hash_func)
        htable->hash_func = hash_func;
    else
        htable->hash_func = default_time33_hash;

    return htable;
}

/* 重新hash函数 */
static void rehash(hash_table *htable)
{
    if (htable->nslot == MAX_HASH_SLOT_SIZE)
        return;
    
    unsigned int new_nslot = htable->nslot * DEFAULT_REHASH_TIME; /* 默认负载因子是2， 这里调整为4，效果应该好点 */
    hash_node **new_slots = zalloc(sizeof(hash_node *) * new_nslot);
    unsigned int new_nelement = 0;
    
    unsigned int index;
    for (index =0; index < htable->nslot; index++) {
        hash_node *head = htable->slots[index];
        if (!head)
            continue;
        
        hash_node *p = NULL;
        while(head) {
            p = head;
            head = head->next;
            
            unsigned int idx = p->hval & (new_nslot - 1);
            if (new_slots[idx])
                p->next = new_slots[idx];
            else
                p->next = NULL;
            
            new_slots[idx] = p;
            new_nelement++;
        }
    }
    
    free(htable->slots);
    htable->slots = new_slots;
    htable->nslot = new_nslot;
    htable->nelement = new_nelement;
}

/* 哈希插入 */
void hash_insert(hash_table *htable, void *key, int klen, void *value)
{
    unsigned int h = htable->hash_func(key, klen);
    unsigned int idx = h & (htable->nslot - 1);
    hash_node *p = htable->slots[idx];
    
    /* 冲突厉害的时候，这里耗时较为严重 */
    while (p) {
        if (0 == htable->hkey_compare_func(key, klen, p->key, p->klen)) {
			if (htable->hdel_func)
                htable->hdel_func(p->val);
            p->val = value;
            return;
        }
        p = p->next;
    }
    
    hash_node *node = zalloc(sizeof(hash_node));
    node->hval = h;
    node->key = zalloc(klen);
    (void)memcpy(node->key, key, klen);
    node->klen = klen;
    node->val = value;
    
    node->next = htable->slots[idx];
    htable->slots[idx] = node;
    htable->nelement++;
    
    if (htable->nelement > htable->nslot * htable->load_factor)
        rehash(htable);
}

/* 哈希删除 */
void hash_delete(hash_table *htable, void *key, int klen)
{
    unsigned int h = htable->hash_func(key, klen);
    unsigned int idx = h & (htable->nslot - 1);
    hash_node *p = htable->slots[idx];
    hash_node *q = NULL;
    
    while (p) {
        if (0 == htable->hkey_compare_func(key, klen, p->key, p->klen)) {
            if (q) 
                q->next = p->next;
            else
                htable->slots[idx] = p->next;
            
            htable->hdel_func(p->val);
            free(p->key);
            free(p);
            
            htable->nelement--;
            break;
        }
        q = p;
        p = p->next;
    }
}

/* 哈希查找 */
int hash_search(hash_table *htable, void *key, int klen, void **value)
{
    unsigned int index = 0;
    while (index < htable->nslot) {
        hash_node *p = htable->slots[index];
        while (p) {
            if (0 == htable->hkey_compare_func(key, klen, p->key, p->klen)) {
                *value = p->val;
                return 0;
            }
            p = p->next;
        }
        index++;
    }
    return -1;
}

/* 哈希遍历 */
int hash_walk(hash_table *htable, hash_walk_func fn, void *user_data)
{
    int ret = 0;
    unsigned int index = 0;
    
    if (!fn)
        return -1;
    
    while (index < htable->nslot) {
        hash_node *p = htable->slots[index];
        while (p) {
            ret = fn(p->key, p->klen, p->val, user_data);
            if (ret)
                return ret;
            
            p = p->next;
        }
        index++;
    }
    return ret;
}

/* 哈希销毁 */
void hash_destroy(hash_table *htable)
{
    unsigned int index = 0;
    while (index < htable->nslot) {
        hash_node *p = htable->slots[index];
        hash_node *q = NULL;
        while (p) {
            q = p;
            p = p->next;
            
            if (htable->hdel_func) 
                htable->hdel_func(q->val);
            free(q->key);
            free(q);
        }
        index++;
    }
	free(htable->slots);
	free(htable);
}

#ifdef __TEST
#include <time.h>
static void data_free(void *data)
{
    if(data)
        free(data);
}

static void gen_rand_str(char * buff, int n)
{
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    srand(clock());
    for (int i = 0; i < n - 1; i++)
    {
        buff[i] = metachar[rand() % 62]; 
    }
    buff[n - 1] = '\0';
}

static void hash_slot_dump(hash_table *htable)
{
    unsigned int index = 0; 
    while (index < htable->nslot) {
        int count = 0;
        hash_node *p = htable->slots[index];
        while (p) {
            count++;
            p = p->next;
        }
		index++;
		printf("%d,%d\n", index, count);
	}
}

int main() 
{
    hash_table *htable = hash_create(data_free, NULL);
    
    for (unsigned int i = 0; i < 1000000; i++) {
        char *data = zalloc(64);
        gen_rand_str(data, 64);
        char *key = zalloc(16);
        gen_rand_str(key, 16);
		hash_insert(htable, key, 16, data);
    }
    hash_slot_dump(htable);
}

#endif /* __TEST */
