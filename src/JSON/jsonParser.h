#ifndef JSON_PARSER
#define JSON_PARSER
#include <cjson/cJSON.h>
#include <stdbool.h>

typedef enum {NO_ERROR, ERROR} td_jsonError;

typedef cJSON *td_json;

td_json jsonParse(char *);

td_json getJSONObject(td_json json, char *field, td_jsonError *error);

int getJSONInt(td_json json, char *field, td_jsonError *error);

void freeJson(td_json);

#endif