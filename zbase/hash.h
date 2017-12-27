/**
 * \file hash.h
 * \brief 哈希表接口定义
 *
 */

#ifndef __HASH_H__
#define __HASH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * 哈希表结构
 */
struct hash_table;
typedef struct hash_table hash_table;

/**
 * 释放哈希表私有数据的函数类型
 * \param priv_data 私有数据指针
 */
typedef void (*hash_free_func_t)(void *priv_data);

/**
 * 哈希函数类型
 * \param key 哈希表的key
 * \param klen key长度
 * \return hash值
 */
typedef unsigned int (*hash_func_t)(void *key, int klen);

/**
 * 获取哈希表的项数
 * \param htable
 * \return 哈希表目前的项目数量
 */
unsigned int hash_element_count(hash_table *htable);

/**
 * 创建一个哈希表
 * \param hdel_func 释放项目中私有数据的函数
 * \param hkey_func 自定义hash函数
 * \retval NULL 创建失败
 * \retval !NULL 创建成功，hash_table 结构
 */
hash_table *hash_create(hash_free_func_t hdel_func, hash_func_t hash_func);
   
/**
 * key比较函数类型
 * \param key1
 * \param klen1
 * \param key2
 * \param klen2
 * \return 0表示相等，非0表示不等
 */
typedef int (*hash_key_compare_func_t)(void *key1, int klen1, void *key2, int klen2);

/**
 * 创建一个哈希表
 * \param hdel_func 释放项目中私有数据的函数
 * \param hkey_func 自定义hash函数
 * \param hkey_compare_func 哈希key的比较函数
 * \param flag 标记位，可以扩展hash操作的一些行为
 * \retval NULL 创建失败
 * \retval !NULL 创建成功，hash_table 结构
 */
hash_table *hash_create2(hash_free_func_t hdel_func, hash_func_t hash_func, 
                           hash_key_compare_func_t hkey_compare_func, int flag);
   

/**
 * 哈希表项插入操作
 * \param htable 哈希表
 * \param key 键
 * \param klen 键长度
 * \param vlaue 值
 */
void hash_insert(hash_table *htable, void *key, int klen, void *vlaue);

/**
 * 哈希表项删除操作
 * \param htable 哈希表
 * \param key 
 * \param klen
 */
void hash_delete(hash_table *htable, void *key, int klen);

/**
 * 哈希表查找操作
 * \param htable 哈希表
 * \param key 键
 * \param klen 键长
 * \param vlaue 值
 * \return 0表示成功， 非0表示查找失败
 */
int hash_search(hash_table *htable, void *key, int klen, void **value);

/**
 * 遍历函数函数类型，每个key-vlaue调用此函数
 * \param key 键
 * \param klen 键长度
 * \param vlaue 值
 * \param user_data 用户数据
 * \return 0表示正常，非0表示遍历停止，遍历函数会返回此返回值
 */
typedef int (*hash_walk_func)(void *key, int klen, 
                              void *vlaue, void *user_data);

/**
 * 哈希遍历函数
 * \param htable hash_table 结构
 * \param fn hash_walk_func 函数
 * \param user_data 用户私有数据
 * \return 0表示成功，非0表示失败
 */
int hash_walk(hash_table *htable, hash_walk_func fn, void *user_data);

/**
 * 销毁一个哈希表
 * \param htable 哈希表结构
 */
void hash_destroy(hash_table *htable);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HASH_H__ */
