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

ass_container assertions;

void push_assertion(int id, char state) {
	assertions.count++;
	assertions.ids = (int*) realloc(assertions.ids, sizeof(int) * assertions.count);
	assertions.states = (char*) realloc(assertions.states, assertions.count);
	assertions.ids[assertions.count - 1] = id;
	assertions.states[assertions.count - 1] = state;
}

void assert_equals(int expect, int actual) {
	push_assertion(0x1, expect == actual);

}

#endif
