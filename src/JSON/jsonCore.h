#ifndef TS_JSON_CORE
#define TS_JSON_CORE

#include <stdbool.h>

typedef struct td_json *td_json;
typedef struct td_jsonObject *td_jsonObject;
typedef struct td_jsonArray *td_jsonArray;
typedef union  td_jsonNumber td_jsonNumber;
typedef bool td_jsonBoolean;

bool isJSONObject(td_json);

bool isJSONArray(td_json);

bool isJSONInteger(td_json);

bool isJSONFloat(td_json);

bool isJSONNumber(td_json);

bool isJSONString(td_json);

bool isJSONBool(td_json);

bool isJSONArray(td_json);

char *getJSONFieldName(td_json);

int jsonToInt(td_json);

float jsonToFloat(td_json);

char *jsonToString(td_json);

bool jsonToBool(td_json);

td_json *jsonToArray(td_json);

int jsonArrayLength(td_json);

td_json getJSONField(td_json, const char*);

td_json parseJSON(const char *);

void jsonObjectIterate(td_json, void (*callback)(void *, void *, char *), void *);

void destroyJSON(td_json);

#endif
