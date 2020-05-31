#include <stdlib.h>
#include <cjson/cJSON.h>
#include "jsonParser.h"

struct json {
    cJSON *parsed;
};

json jsonParse(char *jsonData) {
    cJSON *parsed = cJSON_Parse(jsonData);

    json content = malloc(sizeof(struct json));
    content -> parsed = parsed;

    return content;
}

void freeJson(json content) {
    cJSON_Delete(content -> parsed);
    free(content);
}