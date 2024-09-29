#include "ctest.h"

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

