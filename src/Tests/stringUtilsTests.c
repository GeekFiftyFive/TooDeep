#include <stdlib.h>
#include "stringUtilsTests.h"
#include "testHelper.h"
#include "../Utils/stringUtils.h"

#define EXPECTED "123456"

int runStringUtilsTests() {
    int testFailures = 0;

    char *actual = stringifyInt(123456);
    testFailures += assertString(EXPECTED, actual, "stringifyInt");

    free(actual);

    return testFailures;
}
