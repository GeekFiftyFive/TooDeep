#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../IO/gameLoader.h"
#include "../../IO/resourceLoader.h"
#include "../../Scripting/luaFunctions.h"

#define ASSET_DIR "/assets/"
#define SCRIPT_DIR "/scripts/"

struct td_scene {
    td_linkedList entities;
    td_hashMap behaviors;
};

struct callbackData {
    int index;
    td_linkedList *layers;
    td_hashMap layerIndexes;
    td_game game;
    td_hashMap behaviors;
    char *entityID;
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

void registerVariableCallback(td_json json, void *data) {
    td_script script = (td_script) data;
    td_jsonType jsonType = getJSONType(json);
    td_script_val_type variableType;
    td_script_val val;

    switch(jsonType) {
        case JSON_NUMBER: {
            // TODO: This is hacky, make it prettier >8-)
            double doubleVal = getJSONDouble(json, NULL, NULL);
            int intVal = getJSONInt(json, NULL, NULL);
            if((double) intVal == doubleVal) {
                variableType = INT;
                val.intVal = intVal;
            } else {
                variableType = FLOAT;
                val.floatVal = (float) doubleVal;
            }
            break;
        }
        case JSON_BOOL:
            variableType = BOOL;
            // FIX-ME Implement method for getting boolean value from JSON and use it here
            val.booleanVal = true;
            break;
        case JSON_STRING:
            variableType = STRING;
            val.stringVal = getJSONString(json, NULL, NULL);
            break;
        default:
            logError("Could not convert JSON type to script variable type");
            variableType = INT;
            val.intVal = 0;
    }

    registerVariable(script, getFieldName(json), variableType, val);
}

void behaviourCallback(td_json json, void *data) {
    char *fieldName = getFieldName(json);
    struct callbackData *dataCast = (struct callbackData*) data;

    char *scriptName = getJSONString(json, "script", NULL);
    char *fullScriptName = malloc(strlen(scriptName) + strlen(SCRIPT_DIR) + 1);
    sprintf(fullScriptName, "%s%s", SCRIPT_DIR, scriptName);
    char *scriptContent = loadPlaintextResource(getResourceLoader(dataCast -> game), fullScriptName);
    td_script script = createScript(scriptContent);
    jsonObjectForEach(json, "variables", registerVariableCallback, script);
    td_script_val entityIDVal;
    entityIDVal.stringVal = dataCast -> entityID;
    registerVariable(script, "entityID", STRING, entityIDVal);

    td_linkedList actionList = getFromHashMap(dataCast -> behaviors, fieldName);

    if(!actionList) {
        actionList = createLinkedList();
        insertIntoHashMap(dataCast -> behaviors, fieldName, actionList, destroyLinkedList);    
    }

    appendWithFree(actionList, script, fullScriptName, destroyScript);
}

// TODO: JSON error handling, refactor
void entityCallback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;

    // Get entity type
    char *entityType = getJSONString(json, "entity_type", NULL);
    td_json entityJSON = getEntity(dataCast -> game, entityType);

    // Get asset name
    char *assetName = getJSONString(entityJSON, "start_look.asset", NULL);
    char *fullAssetName = malloc(strlen(assetName) + strlen(ASSET_DIR) + 1);
    sprintf(fullAssetName, "%s%s", ASSET_DIR, assetName);

    // Load asset using asset name and create renderable
    SDL_Surface *startLook =  loadSurfaceResource(getResourceLoader(dataCast -> game), fullAssetName);
    free(fullAssetName);
    td_renderable renderable = createRenderableFromSurface(getRenderer(dataCast -> game), startLook);

    // Get world position of entity
    float x = (float) getJSONDouble(json, "start_pos.x", NULL);
    float y = (float) getJSONDouble(json, "start_pos.y", NULL);
    td_tuple pos = { x, y };
    setRenderablePosition(renderable, pos);

    // Get entity behavior
    char *entityID = newEntityID(dataCast -> game);
    dataCast -> entityID = entityID;
    jsonObjectForEach(entityJSON, "behavior", behaviourCallback, data);

    // Create entity and add it to layer
    td_entity entity = createEntity(entityID, renderable);
    char *layerName = getJSONString(json, "render_info.layer", NULL); 
    int *layerIndex = getFromHashMap(dataCast -> layerIndexes, layerName);
    td_linkedList layer = dataCast -> layers[*layerIndex];
    append(layer, entity, getEntityID(entity));
}



td_scene buildScene(td_game game, char *sceneName) {
    td_json sceneJson = getScene(game, sceneName);
    td_scene scene = malloc(sizeof(struct td_scene));

    int layerCount = getJSONArrayLength(sceneJson, "layers");
    td_linkedList *layers = malloc(sizeof(td_linkedList) * layerCount);
    td_hashMap layerIndexes = createHashMap(layerCount);
    td_hashMap behaviors = createHashMap(10);

    struct callbackData layerData = { 0, layers, layerIndexes, game, behaviors, NULL };

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
    scene -> behaviors = behaviors;

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}

void executeBehaviorCallback(void *entryData, void *callbackData, char *key) {
    lua_State *state = (lua_State*) callbackData;
    executeScript(state, (td_script) entryData);
}

// TODO: Move this to its own module
const char *keySymToString(SDL_Keycode sym) {
    switch(sym) {
        case SDLK_LEFT:
            return "left_arrow";
        case SDLK_RIGHT:
            return "right_arrow";
        default:
            return "";
    }
}

void executeBehaviors(lua_State *state, td_scene scene, td_hashMap keymap, SDL_Event e) {
    td_linkedList updateBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, "on_update");

    // Handle keyboard events
    // TODO: This should probably be moved to its own module
    switch (e.type) {
        case SDL_KEYDOWN: {
            const char* keyname = keySymToString(e.key.keysym.sym);
            char *action = (char *) getFromHashMap(keymap, (char *) keyname);
            if(!action) {
                break;
            }
            td_linkedList keyBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, action);
            listForEach(keyBehaviors, executeBehaviorCallback, state);
            break;
        }
        default:
            break;
    }

    if(!updateBehaviors) {
        return;
    }
    listForEach(updateBehaviors, executeBehaviorCallback, state);
}

td_entity getEntityByID(td_scene scene, char *ID) {
    return getFromList(scene -> entities, ID);
}

td_linkedList getEntities(td_scene scene) {
    return scene -> entities;
}

void destroyScene(td_scene scene) {
    destroyLinkedList(scene -> entities);
    destroyHashMap(scene -> behaviors);
    free(scene);
}
