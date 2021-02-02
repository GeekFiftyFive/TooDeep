#ifndef TD_EVENT_ATTRIBUTES
#define TD_EVENT_ATTRIBUTES

#include <lua.h>
#include "luaValues.h"

typedef struct td_eventAttributes *td_eventAttributes;

td_eventAttributes createEventAttributes();

void registerEventAttribute(td_eventAttributes, td_scriptValType, td_scriptVal, char *);

void mapToLuaTable(td_eventAttributes, lua_State *);

void destroyEventAttributes(td_eventAttributes);

#endif