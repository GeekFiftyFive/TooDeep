#include <stdio.h>
#include <string.h>
#include "testHelper.h"
#include "linkedListTests.h"
#include "jsonTests.h"
#include "hashMapTests.h"
#include "resourceLoaderTests.h"
#include "tupleTests.h"
#include "stringUtilsTests.h"
#include "stateMachineTests.h"
#include "tmpJSONTests.h"
#include "../IO/logger.h"

int runTests() {
    int testFailures = 0;

    testFailures += runLinkedListTests();
    testFailures += runJsonTests();
    testFailures += runHashMapTests();
    testFailures += runResourceLoaderTests();
    testFailures += runTupleTests();
    testFailures += runStringUtilsTests();
    testFailures += runStateMachineTests();
    testFailures += runTmpJsonTests();

    if(testFailures) {
        logError("%d tests failed!\n", testFailures);
        return testFailures;
    } else {
        logInfo("All tests passed!\n");
    }
    return 0;
};

int assertString(char *expected, char *actual, char *message) {
    int result = 0;

    if(!expected || !actual) {
        if(actual || expected) result = 1;
        return result;
    }

    result = strcmp(expected, actual) == 0 ? 0 : 1;
    if(result) {
        logError("\033[0;31m%s failed! Expected %s, got %s\033[0m\n", message, expected, actual);
    }

    return result;
}