#ifndef TD_LUA_FUNCTIONS
#define TD_LUA_FUNCTIONS

#include <lua.h>

void registerCFunctions(lua_State *state);

void executeScript(lua_State *state, char *script);

#endif