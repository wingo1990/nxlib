#ifndef __ZSORT_H__
#define __ZSORT_H__

typedef int (*compare_fun)(const void *, const void *, const void *);

void insert_sort(void *base, size_t nmemb, size_t size, compare_fun cmp, void *arg);

#endif /* __ZSORT_H__ */
