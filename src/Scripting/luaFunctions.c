#include <lualib.h>
#include <lauxlib.h>
#include "luaFunctions.h"
#include "../State/Entity/entity.h"
#include "../DataStructures/HashMap/hashMap.h"

typedef struct td_script_variable {
    td_script_val_type type;
    td_script_val value;
} *td_script_variable;

struct td_script {
    char *content;
    td_hashMap variables;
};

td_script createScript(char *content) {
    td_script script = malloc(sizeof(struct td_script));
    script -> content = content;
    script -> variables = createHashMap(10);
    return script;
}

void destroyScript(td_script script) {
    destroyHashMap(script -> variables);
}

void registerVariable(td_script script, char *name, td_script_val_type type, td_script_val value) {
    td_script_variable variable = malloc(sizeof(struct td_script_variable));
    variable -> type = type;
    variable -> value  = value;
    insertIntoHashMap(script -> variables, name, variable, free);
}

void executeScript(lua_State *state, td_script script) {
    if (luaL_loadstring(state, script -> content) == LUA_OK) {
        if (lua_pcall(state, 0, 1, 0) == LUA_OK) {
            lua_pop(state, lua_gettop(state));
        }
    }
}

// TODO: Use lua C closure instead of global current scene
int moveEntity(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    td_entity entity = getEntityByID(scene, "0");
    td_renderable renderable = getRenderable(entity);
    updateRenderablePosition(renderable, (td_tuple) { 0.5, 0 } );
    return 0;
}

void registerCFunctions(lua_State *state, td_scene scene) {
    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, moveEntity, 1);
    lua_setglobal(state, "moveEntity");
}
