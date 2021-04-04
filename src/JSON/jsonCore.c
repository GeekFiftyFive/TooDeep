#include <stdlib.h>
#include <string.h>
#include "jsonCore.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../IO/logger.h"

#define bail(object) free(object); object = NULL; break;

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
    if(**input != '"') {
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
        return NULL;
    }

    return string;
}

static td_json parseValue(char **);

static td_json parseObject(char **input) {
    // Attempt to consume open brace
    consumeWhitespace(input);
    if(**input != '{') {
        return NULL;
    }

    (*input)++;

    td_json object = malloc(sizeof(struct td_json));

    // Attempt to consume fields
    while(**input != '}') {
        if(**input == '\0') {
            logError("Expected '}'\n");
        }

        consumeWhitespace(input);

        const char *fieldName = parseString(input);
        if(**input != ':') {
            bail(object);
        }

        logInfo("parseObject string: %s\n", fieldName);
        
        (*input)++;

        if(!fieldName) {
            bail(object);
        }

        td_json value = parseValue(input);
    }

    return object;
}

static td_json parseNumber(char **input) {
    // Attempt to consume whitespace
    consumeWhitespace(input);

    logInfo("parseNumber: %c\n", **input);
}

static td_json parseValue(char **input) {
    // Attempt to parse object
    td_json json = parseObject(input);

    if(json) {
        return json;
    }

    // Attempt to parse number
    json = parseNumber(input);
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
