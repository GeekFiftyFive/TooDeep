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

    if(!cJSON_IsNumber(obj)) {
        logWarn("object at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return INT32_MAX;
    }

    return obj -> valueint;
}

double getJSONDouble(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;
    td_json obj = getJSONObject(json, field, error);

    if(!cJSON_IsNumber(obj)) {
        logWarn("object at %s is not a number!\n", field);
        if(error) *(error) = JSON_ERROR;
        return (double) INT32_MAX;
    }

    return obj -> valuedouble;
}

char *getJSONString(td_json json, char *field, td_jsonError *error) {
    if(error) *(error) = JSON_NO_ERROR;

    td_json obj = getJSONObject(json, field, error);

    if(!cJSON_IsString(obj)) {
        logWarn("object at %s is not a string!\n", field);
        if(error) *(error) = JSON_ERROR;
        return NULL;
    }

    return obj -> valuestring;
}

void jsonArrayForEach(td_json json, char *field, void (*callback)(td_json, void *), void *data) {
    td_json obj = getJSONObject(json, field, NULL);

    if(!cJSON_IsArray(obj)) {
        logWarn("object at %s is not an array!\n", field);
        return;
    }

    int size = cJSON_GetArraySize(obj);

    for(int i = 0; i < size; i++) {
        td_json item = cJSON_GetArrayItem(obj, i);
        callback(item, data);
    }    
}

void freeJson(td_json content) {
    cJSON_Delete(content);
}