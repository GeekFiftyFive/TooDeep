#include "jsonCore.h"
#include "../DataStructures/HashMap/hashMap.h"

typedef enum { OBJECT, ARRAY, NUMBER, BOOLEAN,  STRING } td_jsonType;

struct td_jsonRoot {
    td_jsonType type;
    td_jsonObject object;
    td_jsonArray array;
};

struct td_jsonObject {
    td_hashMap keyValuePairs;
};

struct td_jsonArray {
    int size;
    td_jsonValue *values;
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

struct td_jsonValue {
    td_jsonType type;
    union td_innerValue value;
};
