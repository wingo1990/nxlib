#ifndef __ZBASE_H__
#define __ZBASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
# define NULL ((void *)0)
#endif

typedef long unsigned int size_t;

#define offset(T, m)   ((size_t)&((T *)0)->m)

struct plist_head {
    struct plist_head *next;
    int priority;
};

typedef int (*plist_compare)(struct plist_head *, void *userdata);

void plist_init(struct plist_head *head);
void plist_add(struct plist_head *, struct plist_head *);
void plist_del(struct plist_head *, plist_compare , void *userdata);

#define foreach_plist(head, p)  \
    for(p = (head)->next; p; p = p->next)

#define plist_entry(ptr, type, member) \
    (type *)((size_t)ptr - offset(type, member)) 

#ifdef __cplusplus
}
#endif

#endif //__ZBASE_H__
