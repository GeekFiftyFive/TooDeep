#ifndef TESTS
#define TESTS

#include "../IO/logger.h"

#define assert(A, B, C) (A == B ? 0 : 1); if(A != B) logError("\033[0;31m%s failed!\033[0m\n", C)

int runTests();

int assertString(char *, char *, char *);

#endif