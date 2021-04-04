#ifndef TS_JSON_CORE
#define TS_JSON_CORE

#include <stdbool.h>

typedef struct td_json *td_json;
typedef struct td_jsonObject *td_jsonObject;
typedef struct td_jsonArray *td_jsonArray;
typedef struct td_jsonString *td_jsonString;
typedef union  td_jsonNumber td_jsonNumber;
typedef bool td_jsonBoolean;

td_json parseJSON(const char *);

void dumpJSON(td_json);

void destroyJSON(td_json);

#endif
