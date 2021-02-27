#include <stdlib.h>
#include <string.h>
#include <lualib.h>
#include <lauxlib.h>
#include "gameLoader.h"
#include "fileIO.h"
#include "logger.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../State/Entity/entity.h"
#include "../Utils/stringUtils.h"
#include "../Scripting/luaFunctions.h"
#include "../Events/keyboardEvents.h"
#include "../JSON/Loaders/sceneLoader.h"

#define MANIFEST_NAME "td-game.json"
#define SCENES_PATH "scenes"
#define ENTITIES_PATH "entities"
#define TILESETS_PATH "tilesets"
#define ANIMATIONS_PATH "animations"
#define STATE_MACHINES_PATH "statemachines"

struct td_game {
    td_resourceLoader loader;
    td_renderer renderer;
    td_hashMap scenes;
    td_scene currentScene;
    td_hashMap entities;
    td_hashMap tilesets;
    td_hashMap animations;
    td_hashMap stateMachines;
    td_json manifest;
    int entityCount;
    lua_State *state;
    td_hashMap keymap;
    char *nextStage;
};

struct callbackData {
    td_hashMap map;
    td_resourceLoader loader;
};

void setNextStage(td_game game, char *name) {
    char *nameCpy = malloc(strlen(name) + 1);
    strcpy(nameCpy, name);
    game -> nextStage = nameCpy;
}

void addJsonToHashmapCallback(char *path, void *rawData) {
    struct callbackData *data = (struct callbackData*) rawData;
    char *json = loadStaticPlaintextResource(data -> loader, path);
    td_json parsed = jsonParse(json);
    td_jsonError error = JSON_NO_ERROR;
    char *name = getJSONString(parsed, "name", &error);
    char *nameCpy = (char *) malloc(strlen(name) + 1);
    strcpy(nameCpy, name);
    if(error) logError("Malformed JSON! Must contain name!\n");
    else insertIntoHashMap(data -> map, nameCpy, parsed, freeJson);
}

static char *getAction(td_json json, char* actionName) {
    char *action = getJSONString(json, actionName, NULL);

    if(!action) {
        return NULL;
    }

    char *actionCopy = malloc(strlen(action) + 1);
    strcpy(actionCopy, action);
    return actionCopy;
}

void keymapCallback(td_json json, void *data) {
    td_hashMap keymap = (td_hashMap) data;
    char *fieldName = getFieldName(json);

    char *downAction = getAction(json, "down");
    char *upAction = getAction(json, "up");

    td_keymap map = malloc(sizeof(struct td_keymap));
    map -> up = upAction;
    map -> down = downAction;

    insertIntoHashMap(keymap, fieldName, map, destroyKeymap);
}

td_hashMap loadKeymap(td_resourceLoader loader) {
    td_hashMap keymap = createHashMap(10);
    char *json = loadPlaintextResource(loader, "td-keymap.json");
    if(json) {
        td_json keymapData = jsonParse(json);
        jsonObjectForEach(keymapData, "mapping", keymapCallback, keymap);
        freeJson(keymapData);
    } else {
        logWarn("Could not load td-keymap.json\n");
    }
    return keymap;
}

td_game loadGameFromDirectory(char *path, td_renderer renderer) {
    td_game game = malloc(sizeof(struct td_game));

    game -> loader = createResourceLoader(path);
    game -> renderer = renderer;
    game -> entityCount = 0;

    char *manifest = loadPlaintextResource(game -> loader, MANIFEST_NAME);

    if(!manifest) {
        logError("Cannot load manifest file\n");
        destroyResourceLoader(game -> loader);
        free(game);
        return NULL;
    }

    game -> manifest = jsonParse(manifest);

    game -> scenes = createHashMap(10);
    game -> entities = createHashMap(10);
    game -> tilesets = createHashMap(10);
    game -> animations = createHashMap(10);
    game -> stateMachines = createHashMap(10);
    game -> keymap = loadKeymap(game -> loader);
    game -> nextStage = NULL;

    char *scenePath = concatPath(path, SCENES_PATH);
    struct callbackData mapData = {game -> scenes, game -> loader};
    iterateOverDir(scenePath, true, addJsonToHashmapCallback, &mapData);

    char *entityPath = concatPath(path, ENTITIES_PATH);
    mapData.map = game -> entities;
    iterateOverDir(entityPath, true, addJsonToHashmapCallback, &mapData);

    char *tilesetPath = concatPath(path, TILESETS_PATH);
    mapData.map = game -> tilesets;
    iterateOverDir(tilesetPath, true, addJsonToHashmapCallback, &mapData);

    char *animationPath = concatPath(path, ANIMATIONS_PATH);
    mapData.map = game -> animations;
    iterateOverDir(animationPath, true, addJsonToHashmapCallback, &mapData);

    char *stateMachinePath = concatPath(path, STATE_MACHINES_PATH);
    mapData.map = game -> stateMachines;
    iterateOverDir(stateMachinePath, true, addJsonToHashmapCallback, &mapData);

    td_jsonError err;
    char *startSceneName = getJSONString(game -> manifest, "start_scene", &err);

    if(err == JSON_ERROR) {
        logError("start_scene not defined in manifest\n");
    } else {
        td_json json = (td_json) getFromHashMap(game -> scenes, startSceneName);
        game -> currentScene = loadSceneFromJSON(json, game);
    }

    game -> state = luaL_newstate();
    luaL_openlibs(game -> state);

    registerCFunctions(game -> state, game);

    free(scenePath);
    free(entityPath);

    return game;
}

static void copyCallback(void *elementData, void *callbackData, char *key) {
    td_entity entity = (td_entity) elementData;
    td_renderer renderer = (td_renderer) callbackData;
    appendToRenderQueue(renderer, getRenderable(entity));
}

void copySceneToRenderQueue(td_game game) {
    listForEach(getEntities(game -> currentScene), copyCallback, game -> renderer);
}

static void createDebugRenderableCallback(void *elementData, void *callbackData, char *key) {
    td_boxCollider collider = (td_boxCollider) elementData;
    td_renderer renderer = (td_renderer) callbackData;
    td_tuple pos = getBoxColliderPosition(collider);
    td_tuple size = getBoxColliderDimensions(collider);
    addDebugRect(renderer, pos, size);
}

void createDebugRenderables(td_game game) {
   forAllColliders(game -> currentScene, createDebugRenderableCallback, game -> renderer);
}

void executeTick(td_game game, int delta) {
    updateTicks(game -> currentScene);
    physicsUpdate(game -> currentScene, delta);
    resolveCollisions(game -> currentScene);
    executeUpdateBehaviors(game -> state, game -> currentScene, delta);
    iterateAnimations(game -> currentScene);
    executeTimeouts(game -> state, game -> currentScene);
    executeFiredEvents(game -> state, game -> currentScene);
    if(game -> nextStage) {
        td_json json = getScene(game, game -> nextStage);
        destroyScene(game -> currentScene);
        game -> currentScene = loadSceneFromJSON(json, game);
        free(game -> nextStage);
        game -> nextStage = NULL;
        lua_close(game -> state);
        game -> state = luaL_newstate();
        luaL_openlibs(game -> state);
        registerCFunctions(game -> state, game);
    }
}

void executeEvent(td_game game, SDL_Event e) {
    executeEventBehaviors(game -> state, game -> currentScene, game -> keymap, e);
}

lua_State *getState(td_game game) {
    return game -> state;
}

td_json getManifest(td_game game) {
    return game -> manifest;
}

td_json getScene(td_game game, char* sceneName) {
    return (td_json) getFromHashMap(game -> scenes, sceneName);
}

td_json getEntity(td_game game, char* entityName) {
    return (td_json) getFromHashMap(game -> entities, entityName);
}

td_json getTileset(td_game game, char *tilesetName) {
    return (td_json) getFromHashMap(game -> tilesets, tilesetName);
}

td_json getAnimation(td_game game, char *animationName) {
    return (td_json) getFromHashMap(game -> animations, animationName);
}

td_json getStateMachine(td_game game, char *stateMachineName) {
    return (td_json) getFromHashMap(game -> stateMachines, stateMachineName);
}

td_resourceLoader getResourceLoader(td_game game) {
    return game -> loader;
}

td_renderer getRenderer(td_game game) {
    return game -> renderer;
}

td_scene getCurrentScene(td_game game) {
    return game -> currentScene;
}

void setCurrentScene(td_game game, td_scene scene) {
    game -> currentScene = scene;
}

char *newEntityID(td_game game) {
    char *id = stringifyInt(game -> entityCount);
    game -> entityCount++;
    return id;
}

void destroyGame(td_game game) {
    destroyResourceLoader(game -> loader);
    destroyHashMap(game -> scenes);
    destroyHashMap(game -> entities);
    destroyHashMap(game -> keymap);
    destroyHashMap(game -> tilesets);
    destroyHashMap(game -> animations);
    destroyHashMap(game -> stateMachines);
    destroyScene(game -> currentScene);
    lua_close(game -> state);
    freeJson(game -> manifest);
    free(game);
}