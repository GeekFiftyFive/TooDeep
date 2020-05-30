#include <stdio.h>
#include <string.h>
#include "testHelper.h"
#include "linkedListTests.h"

int runTests() {
    int testFailures = 0;

    testFailures += runLinkedListTests();

    if(testFailures) {
        fprintf(stderr, "%d tests failed!\n", testFailures);
    } else {
        printf("All tests passed!\n");
    }
    return 0;
};

int assertString(char *expected, char *actual, char *message) {
    int result = strcmp(expected, actual) == 0 ? 0 : 1;
    if(result) {
        fprintf(stderr, "\033[0;31m%s failed!\033[0m\n", message);
    }

    return result;
}