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
    failedTests += assert(42, getJSONInt(json, "block_1.value_1", NULL), "getJSONInt no error");

    return 0;
}