#ifndef TD_LUA_FUNCTIONS
#define TD_LUA_FUNCTIONS

#include <lua.h>
#include "luaValues.h"
#include "eventAttributes.h"
#include "../State/Scene/scene.h"
#include "../IO/resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../Renderer/renderer.h"

typedef struct td_script *td_script;

td_script createScript(char *);

void destroyScript(td_script);

void registerVariable(td_script, char *, td_scriptValType, td_scriptVal);

void executeCallback(lua_State *, int);

void registerCFunctions(lua_State *, td_scene, td_resourceLoader, td_renderer, td_hashMap);

void executeScript(lua_State *, td_script, td_eventAttributes);

#endif