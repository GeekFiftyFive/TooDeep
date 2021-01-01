#ifndef TD_LUA_FUNCTIONS
#define TD_LUA_FUNCTIONS

#include <lua.h>
#include "../State/Scene/scene.h"

void registerCFunctions(lua_State *state, td_scene);

void executeScript(lua_State *state, char *script);

#endif