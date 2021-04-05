#ifndef TD_JSON_TEST_HELPER
#define TD_JSON_TEST_HELPER

#define TEST_JSON "{\
                        \"block_1\": {\
                            \"value_1\": 42,\
                            \"value_2\": \"test string\",\
                            \"value_3\": true,\
                            \"value_4\": -73,\
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
                        ],\
                        \"block_3\": {\
                            \"val_1\": 1,\
                            \"val_2\": 2,\
                            \"val_3\": 3\
                        },\
                        \"array_2\": [\"string 1\", \"string 2\"],\
                        \"array_3\": [\
                            [1, 2, 3],\
                            [4, 5, 6]\
                        ]\
                    }"

#endif
