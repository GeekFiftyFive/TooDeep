#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"

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

void entityCallback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;
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

    for(int i = 0; i < layerCount; i++) {
        destroyLinkedList(layers[i]);
    }

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}

void destroyScene(td_scene scene) {
    //TODO: Implement
}
