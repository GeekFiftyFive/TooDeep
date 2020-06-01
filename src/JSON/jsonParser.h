#ifndef JSON_PARSER
#define JSON_PARSER
#include <cjson/cJSON.h>

typedef cJSON *json;

json jsonParse(char *);

void freeJson(json);

#endif