#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "jsonCore.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"

#define bail(object) free(object); object = NULL; break;

typedef enum { OBJECT, ARRAY, FNUMBER, INUMBER, BOOLEAN,  STRING } td_jsonType;

struct td_jsonObject {
    td_hashMap keyValuePairs;
};

struct td_jsonArray {
    int size;
    td_linkedList values;
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
    char *string;
};

struct td_json {
    td_jsonType type;
    union td_innerValue value;
};

bool isJSONObject(td_json json) {
    return json -> type == OBJECT;
}

bool isJSONArray(td_json json) {
    return json -> type = ARRAY;
}

bool isJSONInteger(td_json json) {
    return json -> type == INUMBER;
}

bool isJSONFloat(td_json json) {
    return json -> type == FNUMBER;
}

bool isJSONString(td_json json) {
    return json -> type == STRING;
}

bool isJSONBool(td_json json) {
    return json -> type == BOOLEAN;
}

int jsonToInt(td_json json) {
    if(!isJSONInteger(json) && !isJSONFloat(json)) {
        return 0;
    }

    if(isJSONInteger(json)) {
        return json -> value.number.intVal;
    }

    if(isJSONFloat(json)) {
        return (int) json -> value.number.floatVal;
    }
}

float jsonToFloat(td_json json) {
    if(!isJSONInteger(json) && !isJSONFloat(json)) {
        return NAN;
    }

    if(isJSONInteger(json)) {
        return (float) json -> value.number.intVal;
    }

    if(isJSONFloat(json)) {
        return json -> value.number.floatVal;
    }
}

char *jsonToString(td_json json) {
    if(!isJSONString(json)) {
        // TODO: Consider stringifying other types
        return NULL;
    }

    return json -> value.string;
}

bool jsonToBool(td_json json) {
    if(!isJSONBool(json)) {
        return NULL;
    }

    return json -> value.boolean;
}

td_json getJSONField(td_json json, const char* fieldName) {
    if(!isJSONObject(json)) {
        return NULL;
    }

    td_json field = getFromHashMap(json -> value.object -> keyValuePairs, fieldName);

    if(!field) {
        logWarn("Field %s does not exist!\n", fieldName);
    }

    return field;
}

static void consumeWhitespace(char **input) {
    while (
        **input == ' ' || 
        **input == '\n' ||
        **input == '\r' ||
        **input == '\t'
    ) {
        (*input)++;
    }
}

static const char *parseString(char **input) {
    char *start = *input;

    consumeWhitespace(input);

    if(**input != '"') {
        *input = start;
        return NULL;
    }

    (*input)++;

    // TODO: Dynamically allocate this
    char *string = malloc(20);
    int count = 0;

    while(**input != '"' && count != 19) {
        string[count] = **input;
        (*input)++;
        count++;
    }

    string[count] = '\0';
    (*input)++;

    if(count == 19 && **input != '"') {
        free(string);
        *input = start;
        return NULL;
    }

    return string;
}

static td_json parseValue(char **);

static td_json parseObject(char **input) {
    char *start = *input;

    // Attempt to consume open brace
    consumeWhitespace(input);
    if(**input != '{') {
        *input = start;
        return NULL;
    }

    (*input)++;

    td_json object = malloc(sizeof(struct td_json));
    td_hashMap pairs = createHashMap(10);
    object -> value.object = malloc(sizeof(struct td_jsonObject));
    object -> value.object -> keyValuePairs = pairs;

    // Attempt to consume fields
    while(true) {
        if(**input == '\0') {
            logError("Expected '}'\n");
        }

        consumeWhitespace(input);

        const char *fieldName = parseString(input);
        if(**input != ':') {
            *input = start;
            bail(object);
        }

        logDebug("parseObject string: %s\n", fieldName);
        
        (*input)++;

        if(!fieldName) {
            *input = start;
            destroyHashMap(pairs);
            bail(object);
        }

        td_json value = parseValue(input);

        insertIntoHashMap(pairs, fieldName, value, destroyJSON);

        consumeWhitespace(input);
        if(**input != ',') {
            if(**input != '}') {
                logError("Improperly terminated object\n");
                bail(object);
            } else {
                (*input)++;
                return object;
            }
        }
        (*input)++;
    }

    return NULL;
}

static bool isNumeric(char character) {
    return (character >= '0' && character <= '9') || character == '.';
}

static td_json parseNumber(char **input) {
    char *start = *input;

    // Attempt to consume whitespace
    consumeWhitespace(input);

    // TODO: Dynamically reallocate
    char *string = malloc(20);
    int count = 0;
    bool isFloat = false;

    while(isNumeric(**input) && count != 20) {
        if(**input == '.') {
            if(isFloat) {
                // There are two decimal points, bail out
                *input = start;
                bail(string);
            }
            isFloat = true;
        }
        string[count] = **input;
        count++;
        (*input)++;
    }

    string[count] = '\0';

    consumeWhitespace(input);

    if(!(**input == ',' || **input == '}')) {
        *input = start;
        free(string);
        return NULL;
    }

    td_jsonNumber number;
    td_jsonType type;

    if(isFloat) {
        float floatVal = atof(string);
        number.floatVal = floatVal;
        type = FNUMBER;
    } else {
        int intVal = atoi(string);
        number.intVal = intVal;
        type = INUMBER;
    }

    free(string);

    union td_innerValue innerValue;
    innerValue.number = number;

    td_json json = malloc(sizeof(struct td_json));
    json -> type = type;
    json -> value = innerValue;

    return json;
}

static td_json parseBoolean(char **input) {
    consumeWhitespace(input);

    char *string = malloc(6);
    memcpy(string, *input, 5);
    string[5] = '\0';

    bool isTrue = false;
    bool isFalse = false;

    if(strcmp(string, "false") == 0) {
        isFalse = true;
    } else {
        memcpy(string, *input, 4);
        string[4] = '\0';
        if(strcmp(string, "true") == 0) {
            isTrue = true;
        }
    }

    free(string);

    if(!isTrue && !isFalse) {
        return NULL;
    }

    td_json json = malloc(sizeof(struct td_json));

    json -> type = BOOLEAN;

    if(isTrue) {
        (*input) += 4;
        json -> value.boolean = true;
    }

    if(isFalse) {
        (*input) += 5;
        json -> value.boolean = false;
    }

    return json;
}

static td_json parseArray(char **input) {
    char *start = *input;

    consumeWhitespace(input);

    if(**input != '[') {
        *input = start;
        return NULL;
    }

    td_linkedList list = createLinkedList();
    int size = 0;

    (*input)++;

    while(**input != ']') {
        td_json element = parseValue(input);
        (*input)++;
        consumeWhitespace(input);
        size++;
        appendWithFree(list, element, NULL, destroyJSON);
    }

    (*input)++;

    td_json json = malloc(sizeof(struct td_json));

    json -> type = ARRAY;
    td_jsonArray array = malloc(sizeof(struct td_jsonArray));
    array -> size = size;
    array -> values = list;
    json -> value.array = array;

    return json;
}

static td_json parseValue(char **input) {
    logDebug("Attempting to parse object\n");

    // Attempt to parse object
    td_json json = parseObject(input);

    if(json) {
        return json;
    }

    logDebug("Attempting to parse number\n");

    // Attempt to parse number
    json = parseNumber(input);

    if(json) {
        if(json -> type == FNUMBER) {
            logDebug("floatVal: %f\n", json -> value.number.floatVal);
        }
        if(json -> type == INUMBER) {
            logDebug("intVal: %d\n", json -> value.number.intVal);
        }
        return json;
    }

    logDebug("Attempting to parse string\n");

    // Attempt to parse string
    char *string = parseString(input);
    if(string) {
        logDebug("string: %s\n", string);
        union td_innerValue value;
        value.string = string;
        json = malloc(sizeof(struct td_json));
        json -> type = STRING;
        json -> value = value;
        return json;
    }

    logDebug("Attempting to parse boolean\n");

    // Attempt to parse boolean
    json = parseBoolean(input);

    if(json) {
        if(json -> value.boolean) {
            logDebug("booleanVal: true\n");
        } else {
            logDebug("booleanVal: false\n");
        }

        return json;
    }

    logDebug("Attempting to parse array\n");

    // Attempt to parse array
    json = parseArray(input);
    return json;
}

td_json parseJSON(const char *input) {
    td_json json = parseValue(&input);

    if(json -> type == ARRAY || json -> type == OBJECT) {
        return json;
    }

    destroyJSON(json);

    return NULL;
}

void dumpJSONHashMap(td_json json) {
    printHashMap(json -> value.object -> keyValuePairs);
}

void destroyJSON(td_json json) {
    // TODO: Implement
}
