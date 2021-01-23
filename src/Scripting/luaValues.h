#ifndef TD_LUA_VALUES
#define TD_LUA_VALUES

#include <stdbool.h>

typedef enum { FLOAT, INT, BOOL, STRING, NIL } td_scriptValType;

typedef union td_scriptVal {
    float floatVal;
    int intVal;
    bool booleanVal;
    char *stringVal;
} td_scriptVal;

#endif
