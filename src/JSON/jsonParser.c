#include <stdlib.h>
#include "jsonParser.h"

json jsonParse(char *jsonData) {
    json parsed = cJSON_Parse(jsonData);

    return parsed;
}

void freeJson(json content) {
    cJSON_Delete(content);
}