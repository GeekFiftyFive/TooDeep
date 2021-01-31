#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jsonParser.h"
#include "../IO/logger.h"

#define delim "."

td_json jsonParse(char *jsonData) {
    td_json parsed = cJSON_Parse(jsonData);

    if (!parsed) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (!error_ptr) {
            logError("Error before: %s\n", error_ptr);
            cJSON_Delete(parsed);
            return NULL;
        }
    }

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
        json = cJSON_GetObjectItemCaseSensitive(json, token);
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

    if(!cJSON_IsNumber(obj)) {
        logWarn("field at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return 0;
    }

    return obj -> valueint;
}

bool getJSONBool(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!obj) {
        return false;
    }

    if(!cJSON_IsBool(obj)) {
        logWarn("field at %s is not a boolean!\n", field);
        if(error) *(error) = JSON_ERROR;
        return false;
    }

    return (bool) obj -> valueint;
}

double getJSONDouble(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!obj) {
        return 0;
    }

    if(!cJSON_IsNumber(obj)) {
        logWarn("field at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return (double) 0;
    }

    return obj -> valuedouble;
}

char *getJSONString(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;

    td_json obj = field ? getJSONObject(json, field, error) : json;

    if(!obj) {
        return NULL;
    }

    if(!cJSON_IsString(obj)) {
        logWarn("field at %s is not a string!\n", field);
        if(error) *(error) = JSON_ERROR;
        return NULL;
    }

    return obj -> valuestring;
}

bool jsonFieldExists(td_json json, char *field) {
    td_jsonError error = JSON_NO_ERROR;
    getJSONObject(json, field, &error);
    return (error != JSON_ERROR);
}

bool isJSONFieldArray(td_json json, char *field) {
    td_json obj = getJSONObject(json, field, NULL);
    return cJSON_IsArray(obj);
}

bool isJSONFieldBoolean(td_json json, char *field) {
    td_json obj = getJSONObject(json, field, NULL);
    return cJSON_IsBool(obj);
}

void jsonArrayForEach(td_json json, char *field, void (*callback)(td_json, void *), void *data) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!cJSON_IsArray(obj)) {
        logWarn("field at %s is not an array!\n", field);
        return;
    }

    int size = cJSON_GetArraySize(obj);

    for(int i = 0; i < size; i++) {
        td_json item = cJSON_GetArrayItem(obj, i);
        callback(item, data);
    }    
}

void jsonObjectForEach(td_json json, char *field, void (*callback)(td_json, void *), void *data) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!obj) {
        return;
    }

    if(!cJSON_IsObject(obj)) {
        logWarn("field at %s is not an object!\n", field);
        return;
    }
    
    td_json child = obj -> child;

    while(child) {
        callback(child, data);
        child = child -> next;
    }
}

td_jsonType getJSONType(td_json json) {
    switch(json -> type) {
        case cJSON_True:
        case cJSON_False:
            return JSON_BOOL;
        case cJSON_String:
            return JSON_STRING;
        case cJSON_Number:
            return JSON_NUMBER;
        default:
            logWarn("Could not convert cJSON type to td_jsonType");
            return JSON_OTHER;
    }
}

char *getFieldName(td_json json) {
    return json -> string;
}

int getJSONArrayLength(td_json json, char* field) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!cJSON_IsArray(obj)) {
        logWarn("field at %s is not an array!\n", field);
        return 0;
    }

    return cJSON_GetArraySize(obj);
}

void freeJson(td_json content) {
    cJSON_Delete(content);
}