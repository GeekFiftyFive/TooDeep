#include <lualib.h>
#include <lauxlib.h>
#include "luaFunctions.h"
#include "../State/Scene/scene.h"
#include "../State/Entity/entity.h"

void executeScript(lua_State *state, char *script) {
    if (luaL_loadstring(state, script) == LUA_OK) {
        if (lua_pcall(state, 0, 1, 0) == LUA_OK) {
            lua_pop(state, lua_gettop(state));
        }
    }
}

// TODO: Use lua C closure instead of global current scene
int moveEntity(lua_State *state) {
    td_scene scene = getCurrentScene();
    td_entity entity = getEntityByID(scene, "0");
    td_renderable renderable = getRenderable(entity);
    updateRenderablePosition(renderable, (td_tuple) { 0.5, 0 } );
    return 0;
}

void registerCFunctions(lua_State *state) {
    lua_pushcfunction(state, moveEntity);
    lua_setglobal(state, "moveEntity");
}
