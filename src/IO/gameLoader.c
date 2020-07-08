#include <stdlib.h>
#include "gameLoader.h"
#include "resourceLoader.h"
#include "fileIO.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "logger.h"

#define USER_CONFIG_NAME "td-user-config.json"
#define MANIFEST_NAME "td-game.json"
#define SCENES_PATH "scenes"
#define ENTITIES_PATH "entities"

struct td_game {
    td_resourceLoader loader;
    td_hashMap scenes;
    td_hashMap entities;
    td_json config;
    td_json manifest;
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
    if(error) logError("Malformed JSON! Must contain name!\n");
    else insertIntoHashMap(data -> map, name, parsed, freeJson);
}

td_game loadGameFromDirectory(char *path) {
    td_game game = malloc(sizeof(struct td_game));

    game -> loader = createResourceLoader(path);
    char *config = loadPlaintextResource(game -> loader, USER_CONFIG_NAME);

    if(!config) {
        logError("Cannot load config file\n");
        destroyResourceLoader(game -> loader);
        free(game);
        return NULL;
    }

    game -> config = jsonParse(config);

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

    free(scenePath);
    free(entityPath);

    return game;
}

td_json getConfig(td_game game) {
    return game -> config;
}

td_json getManifest(td_game game) {
    return game -> manifest;
}

void destroyGame(td_game game) {
    destroyResourceLoader(game -> loader);
    destroyHashMap(game -> scenes);
    destroyHashMap(game -> entities);
    freeJson(game -> config);
    freeJson(game -> manifest);
    free(game);
}