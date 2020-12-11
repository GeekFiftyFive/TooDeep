#include <stdlib.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../DataStructures/LinkedList/linkedList.h"

struct td_scene {
    td_linkedList entities;
};

struct callbackData {
    int index;
    td_linkedList *layers;
};

void callback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;
    dataCast -> layers[dataCast -> index] = createLinkedList();
    dataCast -> index++;
}

td_scene buildScene(td_game game, char *sceneName) {
    td_json sceneJson = getScene(game, sceneName);
    td_scene scene = malloc(sizeof(struct td_scene));

    int layerCount = getJSONArrayLength(sceneJson, "layers");
    td_linkedList *layers = malloc(sizeof(td_linkedList) * layerCount);

    struct callbackData layerData = { 0, layers };

    jsonArrayForEach(sceneJson, "layers", callback, &layerData);

    for(int i = 0; i < layerCount; i++) {
        destroyLinkedList(layers[i]);
    }

    free(layers);

    return scene;
}

void destroyScene(td_scene scene) {
    //TODO: Implement
}
