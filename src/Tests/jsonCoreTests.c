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

    td_json value_2 = getJSONField(block_1, "value_2");
    if(!value_2) {
        failedTests += fail("Expected value_2 to exist");
    }
    failedTests += assert(true, isJSONString(value_2), "Expect field \"value_2\" to be string");
    failedTests += assertString("test string", jsonToString(value_2), "Expect string to be evaluated");

    td_json value_3 = getJSONField(block_1, "value_3");
    if(!value_3) {
        failedTests += fail("Expected value_3 to exist");
    }
    failedTests += assert(true, isJSONBool(value_3), "Expect field \"value_3\" to be boolean");
    failedTests += assert(true, jsonToBool(value_3), "Expect value of type boolean to be evaluated");

    td_json block_2 = getJSONField(block_1, "block_2");
    failedTests += assert(true, isJSONObject(block_2), "Expect field \"block_2\" to be object");

    destroyJSON(json);
    return failedTests;
}
