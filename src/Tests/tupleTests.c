#include "tupleTests.h"
#include "testHelper.h"
#include "../DataStructures/Tuple/tuple.h"

int runTupleTests() {
    td_tuple a = { 2.0, 4.0 };
    td_tuple b = { 4.0, 5.0 };
    td_tuple toNormalise = { 4.0, 3.0 };

    int failedTests = 0;

    td_tuple actual = productTuple(a, b);
    failedTests += assert(actual.x, 8.0, "productTuple x");
    failedTests += assert(actual.y, 20.0, "productTuple y");

    actual = multiplyTuple(2.0, a);
    failedTests += assert(actual.x, 4.0, "multiplyTuple x");
    failedTests += assert(actual.y, 8.0, "multiplyTuple y");

    actual = divideTuple(b, a);
    failedTests += assert(actual.x, 2.0, "divideTuple x");
    failedTests += assert(actual.y, 1.25, "divideTuple y");

    actual = addTuple(a, b);
    failedTests += assert(actual.x, 6.0, "addTuple x");
    failedTests += assert(actual.y, 9.0, "addTuple y");

    actual = subtractTuple(a, b);
    failedTests += assert(actual.x, -2.0, "subtractTuple x");
    failedTests += assert(actual.y, -1.0, "subtractTuple y");

    actual = normaliseTuple(toNormalise);
    failedTests += assert((int) (actual.x * 10), 8, "normaliseTuple x");
    failedTests += assert((int) (actual.y * 10), 6, "normaliseTuple y");

    return failedTests;
}