#ifndef __ZSORT_H__
#define __ZSORT_H__

#if __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef long unsigned int size_t;

typedef int (*compare_fun)(const void *, const void *, const void *);

void insert_sort(void *base, size_t nmemb, size_t size, compare_fun cmp, void *arg);

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __ZSORT_H__ */
