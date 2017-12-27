#include "stdio.h"
#include "stdlib.h"
#include "gtest/gtest.h"

#include "zbase.h"

typedef struct {
    struct plist_head plist;
    int mydata;
}My_Data_ST;

int test_compare(struct plist_head *node, void *userdata) 
{
    int t = *(int *)userdata;
    My_Data_ST *data = plist_entry(node, My_Data_ST, plist);
    if (t == data->mydata) {
        return 1;
    } else {
        return 0;
    }
}

TEST(plist, opr)
{
    struct plist_head list;
    plist_init(&list);
    
    printf("start add....\n");
    My_Data_ST *test_data = (My_Data_ST *)malloc(sizeof(My_Data_ST));
    memset(test_data, 0, sizeof(My_Data_ST));
    test_data->plist.priority = 1;
    test_data->mydata = 1;
    plist_add(&list, &(test_data->plist));
    test_data = (My_Data_ST *)malloc(sizeof(My_Data_ST));
    memset(test_data, 0, sizeof(My_Data_ST));
    test_data->plist.priority = 3;
    test_data->mydata = 2;
    plist_add(&list, &(test_data->plist));
    test_data = (My_Data_ST *)malloc(sizeof(My_Data_ST));
    memset(test_data, 0, sizeof(My_Data_ST));
    test_data->plist.priority = 2;
    test_data->mydata = 3;
    plist_add(&list, &(test_data->plist));
    test_data = (My_Data_ST *)malloc(sizeof(My_Data_ST));
    memset(test_data, 0, sizeof(My_Data_ST));
    test_data->plist.priority = 4;
    test_data->mydata = 4;
    plist_add(&list, &(test_data->plist));
    
    struct plist_head *p;
    foreach_plist(&list, p) {
        My_Data_ST *data = plist_entry(p, My_Data_ST, plist); 
        printf("value is %d\n", data->mydata);
    }

    printf("add over..\n");
    printf("start deleting...\n");

    int t1 = 4;
    int t2 = 1;
    int t3 = 3;
    int t4 = 2;
 
    plist_del(&list, test_compare, &t1);
    plist_del(&list, test_compare, &t2);
    plist_del(&list, test_compare, &t3);
    plist_del(&list, test_compare, &t4);

    foreach_plist(&list, p) {
        My_Data_ST *data = plist_entry(p, My_Data_ST, plist); 
        printf("value is %d\n", data->mydata);
    }
    
    printf("delete over...\n");
}

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

static int walk_callback(void *key, int klen, void *value, void *user_data)
{
	int *count = (int *)user_data;
	(*count)++;
	return 0;
}

TEST(hash, opr)
{
    hash_table *htable = hash_create(data_free, NULL);
    #define COUNT 1000 
	char *kvs[COUNT][2] = {0};
    for (int i = 0; i < COUNT; i++) {
        char *data = (char *)malloc(64);
        gen_rand_str(data, 64);
        char *key = (char *)malloc(16);
        gen_rand_str(key, 16);
        hash_insert(htable, key, 16, data);
		kvs[i][0] = key;
		kvs[i][1] = data;
		key = NULL;
	}
	
	int walk_count = 0;
	hash_walk(htable, walk_callback, &walk_count);
	ASSERT_EQ(COUNT, walk_count);
	
	ASSERT_EQ(COUNT, hash_element_count(htable));

	for (int i = 0; i < COUNT; i++) {
		char *val = NULL;
		ASSERT_EQ(0, hash_search(htable, kvs[i][0], 16, (void **)&val));
		ASSERT_EQ(kvs[i][1], val);
	}
	
	for (int i = 0; i < COUNT; i++) {
		char *val = NULL;
		hash_delete(htable, kvs[i][0], 16);
		ASSERT_EQ(COUNT - (i+1), hash_element_count(htable));
		ASSERT_NE(0, hash_search(htable, kvs[i][0], 16, (void **)&val));
	}

	hash_destroy(htable);
	htable = NULL;
	for (int i = 0; i < COUNT; i++) {
		free(kvs[i][0]);
	}
}
