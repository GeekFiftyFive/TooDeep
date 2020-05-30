#ifndef TESTS
#define TESTS

#include <stdio.h>

#define assert(A, B, C) (A == B ? 0 : 1); if(A != B) fprintf(stderr, "\033[0;31m%s failed! Expected %d, got %d\033[0m\n", C, A, B)

int runTests();

int assertString(char *, char *, char *);

#endif