#include <stdlib.h>
#include <string.h>
#include "stringUtilsTests.h"
#include "testHelper.h"
#include "../Utils/stringUtils.h"

#define EXPECTED "123456"
#define CONCATINATED "part 1, part 2, part 3!"

int runStringUtilsTests() {
    int testFailures = 0;

    char *actual = stringifyInt(123456);
    testFailures += assertString(EXPECTED, actual, "stringifyInt large int");

    actual = stringifyInt(1);
    testFailures += assertString("1", actual, "stringifyInt number 1");

    actual = stringifyInt(0);
    testFailures += assertString("0", actual, "stringifyInt number 0");

    free(actual);

    // Test td_string
    td_string string = createString();

    appendString(string, "part 1, ");
    appendString(string, "part 2, ");
    appendString(string, "part 3");
    appendChar(string, '!');

    actual = toCString(string);
    testFailures += assertString(CONCATINATED, actual, "string append");
    testFailures += assert(strlen(CONCATINATED), getStringLength(string), "string length");

    free(actual);

    destroyString(string);

    return testFailures;
}
