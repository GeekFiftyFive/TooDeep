#ifndef JSON_PARSER
#define JSON_PARSER
#include <cjson/cJSON.h>
#include <stdbool.h>

typedef enum {JSON_NO_ERROR, JSON_ERROR} td_jsonError;

typedef cJSON *td_json;

td_json jsonParse(char *);

td_json getJSONObject(td_json, char *, td_jsonError *);

int getJSONInt(td_json, char *, td_jsonError *);

double getJSONDouble(td_json, char *, td_jsonError *);

char *getJSONString(td_json, char *, td_jsonError *);

void jsonArrayForEach(td_json, char *, void (*callback)(td_json, void *), void *);

void freeJson(td_json);

#endif