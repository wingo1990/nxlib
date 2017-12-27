#include "plist.h"

#define INT_MAX  2147483647

void plist_init(struct plist_head *head)
{
    head->next = NULL;
    head->priority = INT_MAX;
}

void plist_add(struct plist_head *head, struct plist_head *node)
{
    struct plist_head *cur, *cur_next;
    
    cur = head;
    cur_next = head->next;
    while(cur_next && (cur_next->priority > node->priority)) {
        cur_next = cur_next->next;
        cur = cur->next;
    }

    cur->next = node;
    node->next = cur_next;
}

void plist_del(struct plist_head *head, 
               plist_compare compare, 
               void *userdata)
{
    struct plist_head *cur_pre, *cur;

    cur_pre = head;
    cur = cur_pre->next;
    while (cur) {
        if (compare(cur, userdata)) {
            cur_pre->next = cur->next;
            cur = NULL;
        } else {
            cur_pre = cur_pre->next;
            cur = cur->next;
        }
    }
}

