#ifndef CTEST_H
#define CTEST_H

#include <stdio.h>
#include <stdlib.h>

#define TEST(NAME) void __ctest_utest_##NAME()

typedef struct __assertions {
	int count;
	int* ids;
	char* states;
} ass_container;

extern ass_container assertions;

void push_assertion(int id, char state);

void assert_equals(int expect, int actual);

#endif
