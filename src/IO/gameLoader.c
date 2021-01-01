#include <stdlib.h>
#include <string.h>
#include <lualib.h>
#include <lauxlib.h>
#include "gameLoader.h"
#include "fileIO.h"
#include "logger.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../State/Scene/scene.h"
#include "../State/Entity/entity.h"
#include "../Utils/stringUtils.h"
#include "../Scripting/luaFunctions.h"

#define MANIFEST_NAME "td-game.json"
#define SCENES_PATH "scenes"
#define ENTITIES_PATH "entities"

struct td_game {
    td_resourceLoader loader;
    td_renderer renderer;
    td_hashMap scenes;
    td_scene currentScene;
    td_hashMap entities;
    td_json manifest;
    int entityCount;
    lua_State *state;
};

struct callbackData {
    td_hashMap map;
    td_resourceLoader loader;
};

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

    char *scenePath = concatPath(path, SCENES_PATH);
    struct callbackData mapData = {game -> scenes, game -> loader};
    iterateOverDir(scenePath, true, addJsonToHashmapCallback, &mapData);

    char *entityPath = concatPath(path, ENTITIES_PATH);
    mapData.map = game -> entities;
    iterateOverDir(entityPath, true, addJsonToHashmapCallback, &mapData);

    td_jsonError err;
    char *startSceneName = getJSONString(game -> manifest, "start_scene", &err);

    if(err == JSON_ERROR) {
        logError("start_scene not defined in manifest\n");
    } else {
        game -> currentScene = buildScene(game, startSceneName);
    }

    game -> state = luaL_newstate();
    luaL_openlibs(game -> state);

    registerCFunctions(game -> state, game -> currentScene);

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

void executeTick(td_game game) {
    executeBehaviors(game -> state, game -> currentScene);
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

td_resourceLoader getResourceLoader(td_game game) {
    return game -> loader;
}

td_renderer getRenderer(td_game game) {
    return game -> renderer;
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
    destroyScene(game -> currentScene);
    lua_close(game -> state);
    freeJson(game -> manifest);
    free(game);
}