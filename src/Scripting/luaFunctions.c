#include <lualib.h>
#include <lauxlib.h>
#include "luaFunctions.h"
#include "../State/Entity/entity.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"

typedef struct td_script_variable {
    char *name;
    td_script_val_type type;
    td_script_val value;
} *td_script_variable;

struct td_script {
    char *content;
    td_linkedList variables;
};

td_script createScript(char *content) {
    td_script script = malloc(sizeof(struct td_script));
    script -> content = content;
    script -> variables = createLinkedList();
    return script;
}

void destroyScript(td_script script) {
    destroyLinkedList(script -> variables);
}

void registerVariable(td_script script, char *name, td_script_val_type type, td_script_val value) {
    td_script_variable variable = malloc(sizeof(struct td_script_variable));
    variable -> type = type;
    variable -> value  = value;
    variable -> name = name;
    appendWithFree(script -> variables, variable, name, free);
}

void pushVariableCallback(void *elementData, void *callbackData, char *key) {
    td_script_variable variable = (td_script_variable) elementData;
    lua_State *state = (lua_State*) callbackData;
    
    switch(variable -> type) {
        case INT:
            lua_pushinteger(state, variable -> value.intVal);
            lua_setglobal(state, key);
            break;
        case FLOAT:
            lua_pushnumber(state, variable -> value.floatVal);
            lua_setglobal(state, key);
            break;
        case STRING:
            lua_pushstring(state, variable -> value.stringVal);
            lua_setglobal(state, key);
            break;
        default:
            logError("Could not set up variable %s\n", key);
    }
}

void executeScript(lua_State *state, td_script script) {
    listForEach(script -> variables, pushVariableCallback, state);

    if (luaL_loadstring(state, script -> content) == LUA_OK) {
        if (lua_pcall(state, 0, 1, 0) == LUA_OK) {
            lua_pop(state, lua_gettop(state));
        }
    }
}

// TODO: Use lua C closure instead of global current scene
int moveEntity(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    const char *entityID = luaL_checkstring(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_entity entity = getEntityByID(scene, (char *) entityID);
    td_renderable renderable = getRenderable(entity);
    updateRenderablePosition(renderable, (td_tuple) { x, y } );
    return 0;
}

void registerCFunctions(lua_State *state, td_scene scene) {
    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, moveEntity, 1);
    lua_setglobal(state, "moveEntity");
}
