#include <stdlib.h>
#include <stdio.h>
#include "jsonCore.h"
#include "../DataStructures/HashMap/hashMap.h"

typedef enum { OBJECT, ARRAY, NUMBER, BOOLEAN,  STRING } td_jsonType;

struct td_jsonObject {
    td_hashMap keyValuePairs;
};

struct td_jsonArray {
    int size;
    td_json *values;
};

union td_jsonNumber {
    int intVal;
    float floatVal;
};

union td_innerValue {
    td_jsonObject object;
    td_jsonArray array;
    td_jsonNumber number;
    td_jsonBoolean boolean;
};

struct td_json {
    td_jsonType type;
    union td_innerValue value;
};

td_json parseJSON(const char *input) {
    return NULL;
}

void dumpJSON(td_json json) {
    
}
