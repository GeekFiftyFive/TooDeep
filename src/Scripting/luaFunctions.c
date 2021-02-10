#include <lualib.h>
#include <lauxlib.h>
#include <string.h>
#include "luaFunctions.h"
#include "../State/Entity/entity.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"
#include "../Renderer/camera.h"
#include "../JSON/Loaders/sceneLoader.h"

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

char *getScriptEntityID(td_script script) {
    td_iterator iterator = getIterator(script -> variables);
    struct td_script_variable *variable;

    while((variable = iteratorNext(iterator))) {
        if(strcmp(variable -> name, "entityID") == 0) {
            return variable -> value.stringVal;
        }
    }

    destroyIterator(iterator);
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

void executeScript(lua_State *state, td_script script, td_eventAttributes eventAttributes) {
    listForEach(script -> variables, pushVariableCallback, state);

    mapToLuaTable(eventAttributes, state);

    if (luaL_loadstring(state, script -> content) == LUA_OK) {
        if (lua_pcall(state, 0, 1, 0) == LUA_OK) {
            lua_pop(state, lua_gettop(state));
        } else {
            logError("There was an error executing the script: %s\n", lua_tostring(state, -1));
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

int luaSetCameraZoom(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    float zoom = luaL_checknumber(state, 2);
    setCameraZoom(camera, zoom);
    return 1;
}

int luaGetCameraZoom(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    float zoom = getCameraZoom(camera);
    lua_pushnumber(state, zoom);
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

int luaGetCameraVelocity(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    td_tuple velocity = getCameraVelocity(camera);
    tupleToTable(state, velocity);
    return 1;
}

int luaSetCameraVelocity(lua_State *state) {
    td_camera camera = (td_camera) lua_topointer(state, 1);
    float x = luaL_checknumber(state, 2);
    float y = luaL_checknumber(state, 3);
    td_tuple velocity = (td_tuple) { x, y };
    setCameraVelocity(camera, velocity);
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
    td_game game = (td_game) lua_topointer(state, lua_upvalueindex(1));

    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *animationName = luaL_checkstring(state, 2);

    td_animation animation = getAnimationFromEntity(entity, animationName);
    td_resourceLoader loader = getResourceLoader(game);
    td_scene scene = getCurrentScene(game);
    td_renderer renderer = getRenderer(game);

    if(!animation) {
        td_json json = getAnimation(game, (char *) animationName);
        logInfo("%s\n", animationName);
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
    bool collided = checkCollisionsWithoutCallbacks(collider, getWorldColliders(scene));
    collided = collided || checkCollisionsWithoutCallbacks(collider, getEntityColliders(scene));

    lua_pushboolean(state, collided);
    return 1;
}

static void pushCollider(lua_State *state, td_boxCollider collider, bool terrain) {
    // Set Entity ID
    lua_createtable(state, 0, 6);
    lua_pushstring(state, "entityID");
    lua_pushstring(state, getParentEntityID(collider));
    lua_settable(state, -3);
    // Set Collider X Position
    lua_pushstring(state, "x");
    lua_pushinteger(state, getBoxColliderPosition(collider).x);
    lua_settable(state, -3);
    // Set Collider Y Position
    lua_pushstring(state, "y");
    lua_pushinteger(state, getBoxColliderPosition(collider).y);
    lua_settable(state, -3);
    // Set Collider Width
    lua_pushstring(state, "w");
    lua_pushinteger(state, getBoxColliderDimensions(collider).x);
    lua_settable(state, -3);
    // Set Collider Height
    lua_pushstring(state, "h");
    lua_pushinteger(state, getBoxColliderDimensions(collider).y);
    lua_settable(state, -3);
    // Set Is Terrain
    lua_pushstring(state, "isTerrain");
    lua_pushboolean(state, terrain);
    lua_settable(state, -3);
    lua_settable(state, -3);
}

int luaGetCollisions(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *colliderName = luaL_checkstring(state, 2);
    td_boxCollider collider = getCollisionHull(entity, (char *) colliderName);
    td_linkedList worldColliders = getCollisions(collider, getWorldColliders(scene));
    td_linkedList entityColliders = getCollisions(collider, getEntityColliders(scene));
    td_iterator iterator = getIterator(worldColliders);
    td_boxCollider collided;

    lua_createtable(state, listLength(worldColliders) + listLength(entityColliders), 0);
    int i = 1;
    while((collided = iteratorNext(iterator))) {
        lua_pushinteger(state, i);
        pushCollider(state, collided, true);
        i++;
    }
    
    destroyIterator(iterator);
    iterator = getIterator(entityColliders);
    while((collided = iteratorNext(iterator))) {
        lua_pushinteger(state, i);
        pushCollider(state, collided, false);
        i++;
    }

    destroyIterator(iterator);
    destroyLinkedList(entityColliders);
    destroyLinkedList(worldColliders);
    return 1;
}

int luaFlipEntityHorizontal(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    bool flip = lua_toboolean(state, 2);
    td_entityFlip flipEnum = flip ? TD_HORIZONTAL_FLIP : TD_NO_FLIP;
    setEntityFlip(entity, flipEnum);
    return 1;
}

int luaGetEntityStateValue(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *name = luaL_checkstring(state, 2);
    td_entityState eState = getEntityState(entity);
    td_scriptValType type = entityStateCheckType(eState, (char *) name);

    switch(type) {
        case NIL:
            lua_pushnil(state);
            break;
        case FLOAT:
            lua_pushnumber(state, getFloatFromState(eState, name));
            break;
        case INT:
            lua_pushnumber(state, getIntFromState(eState, name));
            break;
        case BOOL:
            lua_pushboolean(state, getBoolFromState(eState, name));
            break;
        case STRING:
            lua_pushstring(state, getStringFromState(eState, name));
            break;
    }

    return 1;
}

static void pushInt(lua_State *state, td_entityState eState, char *name) {
    int iVal = lua_tointeger(state, 3);
    td_scriptVal val;
    val.intVal = iVal;
    insertIntoState(eState, name, INT, val);
}

int luaSetEntityStateValue(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *name = luaL_checkstring(state, 2);
    td_entityState eState = getEntityState(entity);

    if(lua_isinteger(state, 3)) {
        pushInt(state, eState, name);
    } else if(lua_isnumber(state, 3)) {
        if(entityStateCheckType(eState, name) == INT) {
            pushInt(state, eState, name);
        } else {
            float fVal = lua_tonumber(state, 3);
            td_scriptVal val;
            val.floatVal = fVal;
            insertIntoState(eState, name, FLOAT, val);
        }
    } else if(lua_isboolean(state, 3)) {
        bool bVal = lua_toboolean(state, 3);
        td_scriptVal val;
        val.booleanVal = bVal;
        insertIntoState(eState, name, BOOL, val);
    } else if(lua_isstring(state, 3)) {
        char *sVal = lua_tostring(state, 3);
        td_scriptVal val;
        val.stringVal = sVal;
        insertIntoState(eState, name, STRING, val);
    }

    return 1;
}

int luaGetEntityGravity(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    float gravityAccel = getEntityGravityAccleration(entity);

    lua_pushnumber(state, gravityAccel);
    return 1;
}

int luaSetEntityGravity(lua_State *state) {
    td_entity entity = (td_entity) lua_topointer(state, 1);
    float gravityAccel = luaL_checknumber(state, 2);
    setEntityGravityAcceleration(entity, gravityAccel);
    return 1;
}

int luaSetTimeout(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    float timeout = luaL_checknumber(state, 2);
    lua_pop(state, 1);
    int reference = luaL_ref(state, LUA_REGISTRYINDEX);

    registerTimeout(scene, reference, timeout);

    return 1;
}

int luaSetStateMachineValue(lua_State *state) {
    td_game game = (td_game) lua_topointer(state, lua_upvalueindex(1));
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *name = luaL_checkstring(state, 2);
    td_stateMachine machine = getAnimationStateMachine(entity);

    char *existingState = getCurrentStateId(machine);

    if(lua_isinteger(state, 3)) {
        int value = lua_tointeger(state, 3);
        updateStateMachineIntCondition(machine, name, value);
    } else if(lua_isnumber(state, 3)) {
        if(isStateMachineVariableInt(machine, name)) {
            int value = lua_tointeger(state, 3);
            updateStateMachineIntCondition(machine, name, value);
        } else {
            float value = lua_tonumber(state, 3);
            updateStateMachineFloatCondition(machine, name, value);
        }
    } else if(lua_isboolean(state, 3)) {
        bool value = lua_toboolean(state, 3);
        updateStateMachineIntCondition(machine, name, value);
    }

    char *animationName = getCurrentStateId(machine);
    td_animation animation = getAnimationFromEntity(entity, animationName);
    td_resourceLoader loader = getResourceLoader(game);
    td_scene scene = getCurrentScene(game);
    td_renderer renderer = getRenderer(game);

    if(!animation) {
        td_json json = getAnimation(game, (char *) animationName);
        animation = createAnimationFromJson(json, loader, renderer);
        addAnimation(entity, animation, (char *) animationName);
        addAnimationToScene(scene, animation, animationName);
    }

    if(strcmp(existingState, animationName) != 0) {
        playAnimation(entity, animation);
    }

    return 1;
}

int luaSetScene(lua_State *state) {
    td_game game = (td_game) lua_topointer(state, lua_upvalueindex(1));
    const char *name = luaL_checkstring(state, 1);

    setNextStage(game, name);

    return 1;
}

static td_eventAttributes getEventAttributes(lua_State *state, int index) {
    if(!lua_istable(state, index)) {
        return NULL;
    }

    td_eventAttributes attributes = createEventAttributes();
    lua_pushvalue(state, index);
    lua_pushnil(state);
    while(lua_next(state, -2)) {
        lua_pushvalue(state, -2);
        td_scriptVal val;
        td_scriptValType type;
        const char *key = lua_tostring(state, -1);
        if(lua_isinteger(state, -2)) {
            val.intVal = lua_tointeger(state, -2);
            type = INT;
        } else if(lua_isnumber(state, -2)) {
            val.floatVal = lua_tonumber(state, -2);
            type = FLOAT;
        } else if(lua_isboolean(state, -2)) {
            val.booleanVal = lua_toboolean(state, -2);
            type = BOOL;
        } else if(lua_isstring(state, -2)) {
            val.stringVal = lua_tostring(state, -2);
            type = STRING;
        }
        lua_pop(state, 2);
        registerEventAttribute(attributes, type, val, key);
    }
    lua_pop(state, 1);

    return attributes;
}

int luaFireEvent(lua_State *state) {
    td_scene scene = (td_scene) lua_topointer(state, lua_upvalueindex(1));
    td_entity entity = (td_entity) lua_topointer(state, 1);
    const char *eventName = luaL_checkstring(state, 2);
    td_eventAttributes attributes = getEventAttributes(state, 3);
    pushFiredEvent(scene, eventName, entity, attributes);
    return 1;
}

void executeCallback(lua_State *state, int reference) {
    lua_rawgeti(state, LUA_REGISTRYINDEX, reference);
    lua_pushvalue(state, 1);

    if(lua_pcall(state, 0, 0, 0)) {
        logError("There was an error executing the callback: %s\n", lua_tostring(state, -1));
    }
}

void registerCFunctions(
    lua_State *state,
    td_game game
) {
    td_scene scene = getCurrentScene(game);

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaGetEntity, 1);
    lua_setglobal(state, "getEntity");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaGetCamera, 1);
    lua_setglobal(state, "getCamera");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaCheckCollision, 1);
    lua_setglobal(state, "checkCollision");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaGetCollisions, 1);
    lua_setglobal(state, "getCollisions");

    lua_pushlightuserdata(state, game);
    lua_pushcclosure(state, luaPlayAnimation, 1);
    lua_setglobal(state, "playAnimation");

    lua_pushlightuserdata(state, game);
    lua_pushcclosure(state, luaSetStateMachineValue, 1);
    lua_setglobal(state, "setStateMachineValue");

    lua_pushlightuserdata(state, game);
    lua_pushcclosure(state, luaSetScene, 1);
    lua_setglobal(state, "setScene");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaSetTimeout, 1);
    lua_setglobal(state, "setTimeout");

    lua_pushlightuserdata(state, scene);
    lua_pushcclosure(state, luaFireEvent, 1);
    lua_setglobal(state, "fireEvent");

    lua_pushcfunction(state, luaGetCameraPosition);
    lua_setglobal(state, "getCameraPosition");

    lua_pushcfunction(state, luaSetCameraPosition);
    lua_setglobal(state, "setCameraPosition");

    lua_pushcfunction(state, luaGetCameraZoom);
    lua_setglobal(state, "getCameraZoom");

    lua_pushcfunction(state, luaSetCameraZoom);
    lua_setglobal(state, "setCameraZoom");

    lua_pushcfunction(state, luaGetCameraVelocity);
    lua_setglobal(state, "getCameraVelocity");

    lua_pushcfunction(state, luaSetCameraVelocity);
    lua_setglobal(state, "setCameraVelocity");

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

    lua_pushcfunction(state, luaGetEntityStateValue);
    lua_setglobal(state, "getEntityStateValue");

    lua_pushcfunction(state, luaSetEntityStateValue);
    lua_setglobal(state, "setEntityStateValue");

    lua_pushcfunction(state, luaSetEntityGravity);
    lua_setglobal(state, "setEntityGravity");

    lua_pushcfunction(state, luaGetEntityGravity);
    lua_setglobal(state, "getEntityGravity");
}
