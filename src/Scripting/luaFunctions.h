#ifndef TD_LUA_FUNCTIONS
#define TD_LUA_FUNCTIONS

#include <lua.h>
#include "../State/Scene/scene.h"
#include "../IO/resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../Renderer/renderer.h"

typedef enum { FLOAT, INT, BOOL, STRING } td_script_val_type;

typedef union td_script_val {
    float floatVal;
    int intVal;
    bool booleanVal;
    char *stringVal;
} td_script_val;

typedef struct td_script *td_script;

td_script createScript(char *);

void destroyScript(td_script);

void registerVariable(td_script, char *, td_script_val_type, td_script_val);

void registerCFunctions(lua_State *, td_scene, td_resourceLoader, td_renderer, td_hashMap);

void executeScript(lua_State *, td_script);

#endif