#ifndef JSON_PARSER
#define JSON_PARSER
#include <cjson/cJSON.h>
#include <stdbool.h>

typedef enum {JSON_NO_ERROR, JSON_ERROR} td_jsonError;

typedef enum {JSON_NUMBER, JSON_BOOL, JSON_STRING, JSON_OTHER} td_jsonType;

typedef cJSON *td_json;

td_json jsonParse(char *);

td_json getJSONObject(td_json, char *, td_jsonError *);

int getJSONInt(td_json, char *, td_jsonError *);

bool getJSONBool(td_json, char *, td_jsonError *);

double getJSONDouble(td_json, char *, td_jsonError *);

char *getJSONString(td_json, char *, td_jsonError *);

bool jsonFieldExists(td_json, char *);

bool isJSONFieldArray(td_json, char *);

bool isJSONFieldBoolean(td_json, char *);

void jsonArrayForEach(td_json, char *, void (*callback)(td_json, void *), void *);

void jsonObjectForEach(td_json, char *, void (*callback)(td_json, void *), void *);

td_jsonType getJSONType(td_json);

char *getFieldName(td_json);

int getJSONArrayLength(td_json, char *);

void freeJson(td_json);

#endif