#include <stdio.h>
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