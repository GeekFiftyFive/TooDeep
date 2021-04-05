#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jsonParser.h"
#include "../IO/logger.h"

#define delim "."

td_json jsonParse(char *jsonData) {
    td_json parsed = parseJSON(jsonData);
    return parsed;
}

td_json getJSONObject(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;

    if(!field) return json;

    char *fieldCopy = malloc(strlen(field) + 1);
    strcpy(fieldCopy, field);
    char *token = strtok(fieldCopy, delim);

    while(token) {
        if(!json) {
            if(error) *(error) = JSON_ERROR;
            free(fieldCopy);
            return NULL;
        }
        json = getJSONField(json, token);
        token = strtok(NULL, delim);
    }

    if(!json && error) *(error) = JSON_ERROR;

    free(fieldCopy);
    return json; 
}

int getJSONInt(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!obj) {
        return 0;
    }

    if(!isJSONNumber(obj)) {
        logWarn("field at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return 0;
    }

    return jsonToInt(obj);
}

bool getJSONBool(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!obj) {
        return false;
    }

    if(!isJSONBool(obj)) {
        logWarn("field at %s is not a boolean!\n", field);
        if(error) *(error) = JSON_ERROR;
        return false;
    }

    return jsonToBool(obj);
}

// TODO change name and type signature to float
double getJSONDouble(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!obj) {
        return 0;
    }

    if(!isJSONNumber(obj)) {
        logWarn("field at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return (double) 0;
    }

    return jsonToFloat(obj);
}

char *getJSONString(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;

    td_json obj = field ? getJSONObject(json, field, error) : json;

    if(!obj) {
        return NULL;
    }

    if(!isJSONString(obj)) {
        logWarn("field at %s is not a string!\n", field);
        if(error) *(error) = JSON_ERROR;
        return NULL;
    }

    return jsonToString(obj);
}

bool jsonFieldExists(td_json json, char *field) {
    td_jsonError error = JSON_NO_ERROR;
    getJSONObject(json, field, &error);
    return (error != JSON_ERROR);
}

bool isJSONFieldArray(td_json json, char *field) {
    td_json obj = getJSONObject(json, field, NULL);
    return isJSONArray(obj);
}

bool isJSONFieldBoolean(td_json json, char *field) {
    td_json obj = getJSONObject(json, field, NULL);
    return isJSONBool(obj);
}

void jsonArrayForEach(td_json json, char *field, void (*callback)(td_json, void *), void *data) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!isJSONArray(obj)) {
        logWarn("field at %s is not an array!\n", field);
        return;
    }

    int size = jsonArrayLength(obj);
    td_json *values = jsonToArray(obj);

    for(int i = 0; i < size; i++) {
        td_json item = values[i];
        callback(item, data);
    }    
}

// TODO rename
void jsonObjectForEach(td_json json, char *field, void (*callback)(td_json, void *), void *data) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!obj) {
        return;
    }

    if(!isJSONObject(obj)) {
        logWarn("field at %s is not an object!\n", field);
        return;
    }
    
    jsonObjectIterate(obj, callback, data);
}

td_jsonType getJSONType(td_json json) {
    if(isJSONNumber(json)) {
        return JSON_NUMBER;
    }

    if(isJSONBool(json)) {
        return JSON_BOOL;
    }

    if(isJSONString(json)) {
        return JSON_STRING;
    }

    return JSON_OTHER;
}

//TODO: This seems redundant now, swap usages over to jsonCore method
char *getFieldName(td_json json) {
    logWarn("DEPRECATED: Use getJSONFieldName instead of getFieldName\n");
    return getJSONFieldName(json);
}

int getJSONArrayLength(td_json json, char* field) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!isJSONArray(obj)) {
        logWarn("field at %s is not an array!\n", field);
        return 0;
    }

    return jsonArrayLength(obj);
}

//TODO: This seems redundant now, swap usages over to jsonCore method
void freeJson(td_json content) {
    logWarn("DEPRECATED: Use destroyJSON instead of freeJson\n");
    destroyJSON(content);
}