#ifndef TESTS
#define TESTS

#include <stdio.h>

#define assert(A, B, C) (A == B ? 0 : 1); if(A != B) fprintf(stderr, "%s failed!\n", C)

int runTests();

#endif