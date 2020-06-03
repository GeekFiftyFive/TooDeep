#include <stdio.h>
#include "jsonTests.h"
#include "testHelper.h"
#include "../JSON/jsonParser.h"

#define TEST_JSON "{\
                        \"block_1\": {\
                            \"value_1\": 42\
                        }\
                    }"

int runJsonTests() {
    td_json json = jsonParse(TEST_JSON);
    int failedTests = 0;

    // Get an int value from JSON
    td_jsonError error;
    failedTests += assert(42, getJSONInt(json, "block_1.value_1", &error), "getJSONInt no error");
    failedTests += assert(NO_ERROR, error, "Non error response for valid fetch");

    // Ensure error value is populated
    getJSONInt(json, "invalid_field", &error);
    failedTests += assert(ERROR, error, "Error value is populated");

    return 0;
}