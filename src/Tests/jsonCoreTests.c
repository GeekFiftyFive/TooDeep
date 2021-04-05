#include <stdlib.h>
#include "jsonCoreTests.h"
#include "testHelper.h"
#include "jsonTestHelper.h"
#include "../JSON/jsonCore.h"

int runjsonCoreTests() {
    td_json json = parseJSON(TEST_JSON);
    int failedTests = 0;

    // Interrogate top level object
    failedTests += assert(true, isJSONObject(json), "Expect top JSON to be object");

    td_json block_1 = getJSONField(json, "block_1");
    failedTests += assert(true, isJSONObject(block_1), "Expect field \"block_1\" to be object");

    td_json array = getJSONField(json, "array");
    failedTests += assert(true, isJSONArray(array), "Expect field \"array\" to be array");

    // Interrogate fields
    td_json value_1 = getJSONField(block_1, "value_1");
    if(!value_1) {
        failedTests += fail("Expected value_1 to exist");
    }
    failedTests += assert(true, isJSONInteger(value_1), "Expect field \"value_1\" to be number with integer type");
    failedTests += assert(42, jsonToInt(value_1), "Expect number value of type int to be evaluated");

    destroyJSON(json);
    return failedTests;
}
