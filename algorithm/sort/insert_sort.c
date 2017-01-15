#include <stdlib.h>
#include <string.h>
#include "zsort.h"

#define AR_OFFSET(base, size, offset) ((char *)(base) + ((size) * (offset)))

void insert_sort(void *base, size_t nmemb, size_t size, compare_fun cmp, void *arg)
{
	if (nmemb == 0) 
		return;

	size_t j = 1, i;
	void *key = malloc(size);
	
	while (j < nmemb) {
		memcpy(key, AR_OFFSET(base, size, j), size);
		i = j - 1;
		
		while (i >= 0 && (*cmp)(AR_OFFSET(base, size, i), key, arg) > 0) {
			memcpy(AR_OFFSET(base, size, i + 1), AR_OFFSET(base, size, i), size);
			i = i - 1;
		}
		memcpy(AR_OFFSET(base, size, i + 1), key, size);
		j++;
	}

	free(key);
}

