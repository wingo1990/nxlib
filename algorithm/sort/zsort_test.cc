#include "zsort.h"
#include "stdio.h"
#include "stdlib.h"
#include "gtest/gtest.h"

int int_compare(const void *a, const void *b, const void *args)
{
	int f = *(int *)a;
	int e = *(int *)b;
	return (f - e);
}

TEST(SORT, INSERT_SORT)
{
	int sorta[] = {1, 3, 5, 7, 9, 54, 77, 89, 100};
	int a[] 	= {5, 7, 1, 9, 100, 54, 89, 3, 77};	
	size_t nmemb = sizeof(a)/sizeof(a[0]);
	insert_sort(a, nmemb, sizeof(a[0]), int_compare, NULL);
	for (size_t i = 0; i < nmemb; i++) {
		EXPECT_EQ(a[i], sorta[i]);
	}
}
