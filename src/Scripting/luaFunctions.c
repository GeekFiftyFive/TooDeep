#include <lualib.h>
#include <lauxlib.h>
#include "luaFunctions.h"
#include "../State/Entity/entity.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"
#include "../Renderer/camera.h"

typedef struct td_script_variable {
    char *name;
    td_scriptValType type;
    td_scriptVal value;
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

void registerVariable(td_script script, char *name, td_scriptValType type, td_scriptVal value) {
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

static void tupleToTable(lua_State *state, td_tuple tuple) {
    lua_createtable(state, 0, 2);
    lua_pushstring(state, "x");
    lua_pushnumber(state, tuple.x);
    lua_settable(state, -3);

    lua_pushstring(state, "y");
    lua_pushnumber(state, tuple.y);
    lua_settable(state, -3);
}

int luaGetEntityPosition(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    td_tuple position = getEntityPosition(entity);
    tupleToTable(state, position);
    return 1;
}

int luaSetEntityPosition(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_tuple position = (td_tuple) { x, y };
    setEntityPosition(entity, position);
    return 1;   
}

int luaGetCameraPosition(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    td_tuple position = getCameraPosition(camera);
    tupleToTable(state, position);
    return 1;
}

int luaSetCameraPosition(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_tuple position = (td_tuple) { x, y };
    setCameraPosition(camera, position);
    return 1;   
}

int luaGetEntityVelocity(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    td_tuple velocity = getEntityVelocity(entity);
    tupleToTable(state, velocity);
    return 1;
}

int luaSetEntityVelocity(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_tuple velocity = (td_tuple) { x, y };
    setEntityVelocity(entity, velocity);
    return 1;
}

int luaApplyForceToEntity(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_tuple force = (td_tuple) { x, y };
    applyForceToEntity(entity, force);
    return 1;
}

int luaGetEntity(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    const char *entityID = luaL_checkstring(state, 1);
    td_entity entity = getEntityByID(scene, (char *) entityID);

    lua_pushlightuserdata(state, entity);
    return 1;
}

int luaGetCamera(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    const char *cameraID = luaL_checkstring(state, 1);
    td_camera camera = getCameraByID(scene, (char *) cameraID);

    lua_pushlightuserdata(state, camera);
    return 1;
}

int luaPlayAnimation(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(4));
    td_resourceLoader loader = (td_resourceLoader) lua_topointer(state, lua_upvalueindex(3));
    td_renderer renderer = (td_renderer) lua_topointer(state, lua_upvalueindex(2));
    td_hashMap animations = (td_hashMap) lua_topointer(state, lua_upvalueindex(1));

    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *animationName = luaL_checkstring(state, 2);

    td_animation animation = getAnimationFromEntity(entity, animationName);

    if(!animation) {
        td_json json = (td_json) getFromHashMap(animations, (char *) animationName);
        animation = createAnimationFromJson(json, loader, renderer);
        addAnimation(entity, animation, (char *) animationName);
        addAnimationToScene(scene, animation, animationName);
    }

    playAnimation(entity, animation);
    return 1;
}

int luaCheckCollision(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *colliderName = luaL_checkstring(state, 2);
    td_boxCollider collider = getCollisionHull(entity, (char *) colliderName);
    bool collided = checkWorldCollisions(collider, getWorldColliders(scene));

    lua_pushboolean(state, collided);
    return 1;
}

int luaFlipEntityHorizontal(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    bool flip = lua_toboolean(state, 2);
    td_entityFlip flipEnum = flip ? TD_HORIZONTAL_FLIP : TD_NO_FLIP;
    setEntityFlip(entity, flipEnum);
    return 1;
}

void registerCFunctions(
    lua_State *state,
    td_scene scene,
    td_resourceLoader loader,
    td_renderer renderer,
    td_hashMap animations
) {
    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaGetEntity, 1);
    lua_setglobal(state, "getEntity");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaGetCamera, 1);
    lua_setglobal(state, "getCamera");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaCheckCollision, 1);
    lua_setglobal(state, "checkCollision");

    lua_pushlightuserdata(state, animations);
    lua_pushlightuserdata(state, renderer);
    lua_pushlightuserdata(state, loader);
    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaPlayAnimation, 4);
    lua_setglobal(state, "playAnimation");

    lua_pushcfunction(state, luaGetCameraPosition);
    lua_setglobal(state, "getCameraPosition");

    lua_pushcfunction(state, luaSetCameraPosition);
    lua_setglobal(state, "setCameraPosition");

    lua_pushcfunction(state, luaGetEntityPosition);
    lua_setglobal(state, "getEntityPosition");

    lua_pushcfunction(state, luaSetEntityPosition);
    lua_setglobal(state, "setEntityPosition");

    lua_pushcfunction(state, luaGetEntityVelocity);
    lua_setglobal(state, "getEntityVelocity");

    lua_pushcfunction(state, luaSetEntityVelocity);
    lua_setglobal(state, "setEntityVelocity");

    lua_pushcfunction(state, luaApplyForceToEntity);
    lua_setglobal(state, "applyForceToEntity");

    lua_pushcfunction(state, luaFlipEntityHorizontal);
    lua_setglobal(state, "flipEntityHorizontal");
}
