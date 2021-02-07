#ifndef TD_LUA_FUNCTIONS
#define TD_LUA_FUNCTIONS

#include <lua.h>
#include "luaValues.h"
#include "eventAttributes.h"
#include "../IO/gameLoader.h"

typedef struct td_script *td_script;

td_script createScript(char *);

char *getScriptEntityID(td_script);

void destroyScript(td_script);

void registerVariable(td_script, char *, td_scriptValType, td_scriptVal);

void executeCallback(lua_State *, int);

void registerCFunctions(lua_State *, td_game);

void executeScript(lua_State *, td_script, td_eventAttributes);

#endif