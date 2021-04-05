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

    // Interragote fields


    destroyJSON(json);
    return failedTests;
}
