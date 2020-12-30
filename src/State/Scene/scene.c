#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../IO/gameLoader.h"
#include "../../IO/resourceLoader.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../Entity/entity.h"

#define ASSET_DIR "/assets/"

struct td_scene {
    td_linkedList entities;
};

struct callbackData {
    int index;
    td_linkedList *layers;
    td_hashMap layerIndexes;
    td_game game;
};

void layerCallback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;
    dataCast -> layers[dataCast -> index] = createLinkedList();
    char *layerName = getJSONString(json, NULL, NULL);
    
    // Create pointer to index
    int *index = malloc(sizeof(int));
    *index = dataCast -> index;

    // Copy name
    char *layerNameCpy = malloc(strlen(layerName) + 1);
    strcpy(layerNameCpy, layerName);

    // FIXME: This looks like this should leak, but freeing causes double free
    insertIntoHashMap(dataCast -> layerIndexes, layerNameCpy, index, NULL);
    dataCast -> index++;
}

// TODO: JSON error handling
void entityCallback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;
    char *entityType = getJSONString(json, "entity_type", NULL);
    td_json entityJSON = getEntity(dataCast -> game, entityType);
    char *assetName = getJSONString(entityJSON, "start_look.asset", NULL);
    char *fullAssetName = malloc(strlen(assetName) + strlen(ASSET_DIR) + 1);
    sprintf(fullAssetName, "%s%s", ASSET_DIR, assetName);
    SDL_Surface *startLook =  loadSurfaceResource(getResourceLoader(dataCast -> game), fullAssetName);
    free(fullAssetName);
    td_renderable renderable = createRenderableFromSurface(getRenderer(dataCast -> game), startLook);
    td_entity entity = createEntity(renderable);
    char *layerName = getJSONString(json, "render_info.layer", NULL); 
    int *layerIndex = getFromHashMap(dataCast -> layerIndexes, layerName);
    td_linkedList layer = dataCast -> layers[*layerIndex];
    append(layer, entity, newEntityID(dataCast -> game));
}

td_scene buildScene(td_game game, char *sceneName) {
    td_json sceneJson = getScene(game, sceneName);
    td_scene scene = malloc(sizeof(struct td_scene));

    int layerCount = getJSONArrayLength(sceneJson, "layers");
    td_linkedList *layers = malloc(sizeof(td_linkedList) * layerCount);
    td_hashMap layerIndexes = createHashMap(layerCount);

    struct callbackData layerData = { 0, layers, layerIndexes, game };

    jsonArrayForEach(sceneJson, "layers", layerCallback, &layerData);
    jsonArrayForEach(sceneJson, "entities", entityCallback, &layerData);

    td_linkedList entities = createLinkedList();

    for(int i = 0; i < layerCount; i++) {
        appendList(entities, layers[i]);
        destroyLinkedList(layers[i]);
    }

    // TODO: Find a more graceful way of doing this
    dangerouslyAddFreeFunc(entities, destroyEntity);

    scene -> entities = entities;

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}

td_linkedList getEntities(td_scene scene) {
    return scene -> entities;
}

void destroyScene(td_scene scene) {
    destroyLinkedList(scene -> entities);
}
