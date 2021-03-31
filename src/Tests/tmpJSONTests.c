#include "tmpJSONTests.h"
#include "testHelper.h"
#include "../JSON/jsonCore.h"

#define TEST_JSON "{\
                        \"block_1\": {\
                            \"value_1\": 42,\
                            \"value_2\": \"test string\",\
                            \"block_2\": {\
                                \"double\": 3.1415\
                            }\
                        },\
                        \"array\": [\
                            {\
                                \"val\": 1\
                            },\
                            {\
                                \"val\": 2\
                            },\
                            {\
                                \"val\": 3\
                            }\
                        ]\
                    }"

int runTmpJsonTests() {
    td_json json = parseJSON(TEST_JSON);
    dumpJSON(json);
    return 0;
}
