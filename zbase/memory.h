#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *zalloc(size_t size)
{
    void *m = malloc(size);
    if(!m) {
        fprintf(stderr, "malloc failed: exit!");
        exit(-1);
    }
    memset(m, 0, size);
	return m;
}

#endif /* __MEMORY_H__ */

