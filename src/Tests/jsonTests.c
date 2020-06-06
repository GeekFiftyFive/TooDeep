#include <stdio.h>
#include "jsonTests.h"
#include "testHelper.h"
#include "../JSON/jsonParser.h"
#include "../IO/logger.h"

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
    failedTests += assert(JSON_NO_ERROR, error, "Non error response for valid fetch");

    // Get JSON object from JSON
    td_json subObject = getJSONObject(json, "block_1", NULL);
    failedTests += assert(42, getJSONInt(subObject, "value_1", NULL), "Get value from sub object");

    // Ensure error value is populated

    // Avoid spamming logs
    setLevelEnabled(LOG_WARN, false);    

    getJSONInt(json, "invalid_field", &error);
    failedTests += assert(JSON_ERROR, error, "Error value is populated");

    setLevelEnabled(LOG_WARN, true);

    return 0;
}