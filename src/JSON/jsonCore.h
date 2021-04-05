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

td_json getJSONField(td_json, const char*);

td_json parseJSON(const char *);

void destroyJSON(td_json);

#endif
