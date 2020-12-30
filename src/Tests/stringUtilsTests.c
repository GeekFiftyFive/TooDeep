#include <stdlib.h>
#include "stringUtilsTests.h"
#include "testHelper.h"
#include "../Utils/stringUtils.h"

#define EXPECTED "123456"

int runStringUtilsTests() {
    int testFailures = 0;

    char *actual = stringifyInt(123456);
    testFailures += assertString(EXPECTED, actual, "stringifyInt large int");

    actual = stringifyInt(1);
    testFailures += assertString("1", actual, "stringifyInt number 1");

    actual = stringifyInt(0);
    testFailures += assertString("0", actual, "stringifyInt number 0");

    free(actual);

    return testFailures;
}
