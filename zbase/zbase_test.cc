#include "zbase.h"
#include "stdio.h"
#include "stdlib.h"
#include "gtest/gtest.h"

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
