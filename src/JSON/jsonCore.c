#include <stdlib.h>
#include <string.h>
#include "jsonCore.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../IO/logger.h"

#define bail(object) free(object); object = NULL; break;

typedef enum { OBJECT, ARRAY, FNUMBER, INUMBER, BOOLEAN,  STRING } td_jsonType;

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
    char *string;
};

struct td_json {
    td_jsonType type;
    union td_innerValue value;
};

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

        logInfo("parseObject string: %s\n", fieldName);
        
        (*input)++;

        if(!fieldName) {
            *input = start;
            bail(object);
        }

        td_json value = parseValue(input);
        consumeWhitespace(input);
        if(**input != ',') {
            if(**input != '}') {
                logError("Improperly terminated object\n");
                bail(object);
            } else {
                return object;
            }
        }
        (*input)++;
    }

    return object;
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

    if(**input != ',') {
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

static td_json parseValue(char **input) {
    // Attempt to parse object
    td_json json = parseObject(input);

    if(json) {
        return json;
    }

    // Attempt to parse number
    json = parseNumber(input);

    if(json) {
        if(json -> type == FNUMBER) {
            logInfo("floatVal: %f\n", json -> value.number.floatVal);
        }
        if(json -> type == INUMBER) {
            logInfo("intVal: %d\n", json -> value.number.intVal);
        }
        return json;
    }

    logInfo("Attempting to parse string\n");

    // Attempt to parse string
    char *string = parseString(input);
    if(string) {
        logInfo("string: %s\n", string);
        union td_innerValue value;
        value.string = string;
        json = malloc(sizeof(struct td_json));
        json -> type = STRING;
        json -> value = value;
        return json;
    }
}

td_json parseJSON(const char *input) {
    td_json json = parseValue(&input);

    if(json -> type == ARRAY || json -> type == OBJECT) {
        return json;
    }

    destroyJSON(json);

    return NULL;
}

void dumpJSON(td_json json) {
    if(!json) {
        logInfo("JSON Null\n");
    } else {
        logInfo("There's something here!\n");
    }
}

void destroyJSON(td_json json) {
    // TODO: Implement
}
