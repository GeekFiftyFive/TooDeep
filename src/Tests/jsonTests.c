#include <stdio.h>
#include "jsonTests.h"
#include "testHelper.h"
#include "jsonTestHelper.h"
#include "../JSON/jsonParser.h"
#include "../IO/logger.h"

void jsonArrayTestCallback(td_json json, void *data) {
    int *intData = (int *) data;
    int value = getJSONInt(json, "val", NULL);
    *(intData) += value;
}

void jsonObjectTestCallback(td_json json, void *data) {
    int *intData = (int *) data;
    int value = getJSONInt(json, NULL, NULL);
    *(intData) += value;
}

int runJsonTests() {
    td_json json = jsonParse(TEST_JSON);
    int failedTests = 0;

    // Get an int value from JSON
    td_jsonError error;
    failedTests += assert(42, getJSONInt(json, "block_1.value_1", &error), "getJSONInt no error");
    failedTests += assert(JSON_NO_ERROR, error, "Non error response for valid int fetch");

    // Get JSON object from JSON
    td_json subObject = getJSONObject(json, "block_1", NULL);
    failedTests += assert(42, getJSONInt(subObject, "value_1", NULL), "Get value from sub object");

    // Get String value from JSON
    failedTests += assertString("test string", getJSONString(json, "block_1.value_2", &error), "getJSONString no error");
    failedTests += assert(JSON_NO_ERROR, error, "Non error response for valid string fetch");

    // Get String value when no field given
    td_json stringObject = getJSONObject(json, "block_1.value_2", &error);
    failedTests += assertString("test string", getJSONString(stringObject, NULL, &error), "getJSONString empty field no error");
    failedTests += assert(JSON_NO_ERROR, error, "Non error response for valid string fetch");

    // Get a double value from JSON
    failedTests += assert(31415, (int) ((float) getJSONDouble(json, "block_1.block_2.double", &error) * 10000), "getJSONDouble no error");
    failedTests += assert(JSON_NO_ERROR, error, "Non error response for valid double fetch");

    // Call callback function on all elements of a JSON array
    int testData = 0;
    jsonArrayForEach(json, "array", jsonArrayTestCallback, &testData);
    failedTests += assert(6, testData, "callback function called for all elements of a JSON array");

    // Call callback function on all fields in an object
    testData = 0;
    jsonObjectForEach(json, "block_3", jsonObjectTestCallback, &testData);
    failedTests += assert(6, testData, "callback function called for all elements of a JSON object");

    // Ensure error value is populated

    // Avoid spamming logs
    setLevelEnabled(LOG_WARN, false);

    getJSONInt(json, "invalid_field", &error);
    failedTests += assert(JSON_ERROR, error, "Error value is populated");

    setLevelEnabled(LOG_WARN, true);

    return failedTests;
}