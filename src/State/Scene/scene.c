#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../IO/gameLoader.h"
#include "../../IO/resourceLoader.h"
#include "../../Scripting/luaFunctions.h"
#include "../../Events/keyboardEvents.h"
#include "../../DataStructures/Box/box.h"
#include "../../Physics/boxCollision.h"

#define ASSET_DIR "/assets/"
#define SCRIPT_DIR "/scripts/"

struct td_scene {
    td_linkedList entities;
    td_hashMap behaviors;
    td_linkedList mutableColliders;
    td_linkedList immutableColliders;
};

struct callbackData {
    int index;
    td_linkedList *layers;
    td_hashMap layerIndexes;
    td_game game;
    td_hashMap behaviors;
    char *entityID;
    td_linkedList mutableColliders;
    td_linkedList immutableColliders;
};

struct tilesetCallbackData {
    td_game game;
    SDL_Rect tileDimensions;
    td_tuple worldDimensions;
    td_tuple startPosition;
    SDL_Texture *texture;
    td_linkedList layer;
    int xIndex;
    int yIndex;
    bool collision;
    td_linkedList immutableColliders;
};

struct addCollisionHullCallbackData {
    td_entity entity;
    td_linkedList mutableColliders;
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
            // TODO: This is hacky, make it prettier
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

void physicsUpdate(td_scene scene, int delta) {
    listForEach(scene -> entities, entityPhysicsUpdate, &delta);
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

void handleCollision(td_collision collision, void *data) {
    td_entity entity = (td_entity) data;
    td_tuple position = getEntityPosition(entity);
    setEntityPosition(entity, addTuple(position, collision.amount));
    setEntityVelocity(entity, (td_tuple) { 0.0, 0.0 });
}

void addCollisionHullCallback(td_json json, void *data) {
    struct addCollisionHullCallbackData *callbackData = (struct addCollisionHullCallbackData *) data;
    td_tuple entityStartPosition= getEntityPosition(callbackData -> entity);
    float x = (float) getJSONDouble(json, "x", NULL);
    float y = (float) getJSONDouble(json, "y", NULL);
    float w = (float) getJSONDouble(json, "w", NULL);
    float h = (float) getJSONDouble(json, "h", NULL);
    char *name = getJSONString(json, "name", NULL);
    td_box hull = (td_box) {
        x + entityStartPosition.x,
        y + entityStartPosition.y,
        w,
        h
    };
    td_boxCollider collider = createBoxCollider(hull);
    registerBoxColliderCallback(collider, handleCollision);
    registerBoxColliderCallbackData(collider, callbackData -> entity);

    char *nameCpy = malloc(strlen(name) + 1);
    strcpy(nameCpy, name);

    addCollisionHull(callbackData -> entity, collider, nameCpy);
    append(callbackData -> mutableColliders, collider, name);
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

    // Get entity behavior
    char *entityID = newEntityID(dataCast -> game);
    dataCast -> entityID = entityID;
    jsonObjectForEach(entityJSON, "behavior", behaviourCallback, data);

    // Get physics info
    bool gravityEnabled = getJSONBool(entityJSON, "physics.gravity_enabled", NULL);

    // Create entity
    td_entity entity = createEntity(entityID, renderable);
    setEntityPosition(entity, pos);
    enableEntityGravity(entity, gravityEnabled);
    char *layerName = getJSONString(json, "render_info.layer", NULL); 
    int *layerIndex = getFromHashMap(dataCast -> layerIndexes, layerName);

    // Register collision hulls
    struct addCollisionHullCallbackData addCollisionHullCallbackData;
    addCollisionHullCallbackData.entity = entity;
    addCollisionHullCallbackData.mutableColliders = dataCast -> mutableColliders;
    jsonArrayForEach(entityJSON, "physics.collision_hulls", addCollisionHullCallback, &addCollisionHullCallbackData);

    // Add entitiy to corresponding layer
    td_linkedList layer = dataCast -> layers[*layerIndex];
    append(layer, entity, getEntityID(entity));
}

static SDL_Rect getTextureRegion(SDL_Texture *texture, SDL_Rect dimensions, int index) {
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect region = dimensions;
    if(index * dimensions.w > w) {
        // Wrap
        region.x = (index - 1) - (w / dimensions.w);
        region.y = h * ((int) (index / (w / dimensions.w)));
    } else {
        region.x = (index - 1) * dimensions.w;
        region.y = 0;    
    }
    return region;
}

void tileIndexCallback(td_json json, void *data) {
    int tileNum = getJSONInt(json, NULL, NULL);
    struct tilesetCallbackData *callbackData = (struct tilesetCallbackData*) data;
    if(!tileNum) {
        callbackData -> xIndex++;
        return;
    }
    td_renderable renderable = createRenderableFromTexture(
        getRenderer(callbackData -> game),
        callbackData -> texture
    );
    setRenderableTextureRegion(
        renderable,
        getTextureRegion(
            callbackData -> texture,
            callbackData -> tileDimensions,
            tileNum
        )
    );
    setRenderableSize(renderable, callbackData -> worldDimensions);
    char *entityID = newEntityID(callbackData -> game);
    
    // Create entity and add it to layer
    td_entity entity = createEntity(entityID, renderable);
    td_tuple pos = callbackData -> startPosition;
    td_tuple displacement = productTuple(
        callbackData -> worldDimensions,
        (td_tuple) { callbackData -> xIndex, -callbackData -> yIndex }
    );
    pos = addTuple(pos, displacement);

    setEntityPosition(entity, pos);
    append(callbackData -> layer, entity, getEntityID(entity));

    // Create collision
    td_box hull = (td_box) {
        pos.x,
        pos.y,
        callbackData -> worldDimensions.x,
        callbackData -> worldDimensions.y
    };

    td_boxCollider collider = createBoxCollider(hull);
    char *idCopy = malloc(strlen(entityID) + 1);
    strcpy(idCopy, entityID);
    appendWithFree(callbackData -> immutableColliders, collider, idCopy, free);

    callbackData -> xIndex++;
}

void tileRowCallback(td_json json, void *data) {
    jsonArrayForEach(json, NULL, tileIndexCallback, data);
    struct tilesetCallbackData *callbackData = (struct tilesetCallbackData*) data;
    callbackData -> yIndex++;
    callbackData -> xIndex = 0;
}

void tilesetCallback(td_json json, void *data) {
    struct callbackData *dataCast = (struct callbackData*) data;

    // Get tileset type
    char *tilesetType = getJSONString(json, "tileset", NULL);
    td_json tilesetJSON = getTileset(dataCast -> game, tilesetType);

    // Get asset name
    char *assetName = getJSONString(tilesetJSON, "source", NULL);
    char *fullAssetName = malloc(strlen(assetName) + strlen(ASSET_DIR) + 1);
    sprintf(fullAssetName, "%s%s", ASSET_DIR, assetName);

    // Load asset using asset name and create texture
    SDL_Surface *surface =  loadSurfaceResource(getResourceLoader(dataCast -> game), fullAssetName);
    free(fullAssetName);
    SDL_Texture *texture = surfaceToTexture(getRenderer(dataCast -> game), surface);
    
    // Get additional details about the tiles
    bool collision = getJSONBool(json, "collision", NULL);
    char *layerName = getJSONString(json, "layer", NULL);
    float startX = (float) getJSONDouble(json, "start_point.x", NULL);
    float startY = (float) getJSONDouble(json, "start_point.y", NULL);
    float width = (float) getJSONDouble(json, "tile_size.w", NULL);
    float height = (float) getJSONDouble(json, "tile_size.h", NULL);
    int tileWidth = getJSONInt(tilesetJSON, "dimensions.w", NULL);
    int tileHeight = getJSONInt(tilesetJSON, "dimensions.h", NULL);
    int *layerIndex = getFromHashMap(dataCast -> layerIndexes, layerName);
    td_linkedList layer = dataCast -> layers[*layerIndex];

    // Create data to be called on tiles callbacks
    struct tilesetCallbackData callbackData = {
        dataCast -> game,
        (SDL_Rect) { 0, 0, tileWidth, tileHeight },
        (td_tuple) { width, height },
        (td_tuple) { startX, startY },
        texture,
        layer,
        0,
        0,
        collision,
        dataCast -> immutableColliders
    };

    jsonArrayForEach(json, "tiles", tileRowCallback, &callbackData);
}

td_scene buildScene(td_game game, char *sceneName) {
    td_json sceneJson = getScene(game, sceneName);
    td_scene scene = malloc(sizeof(struct td_scene));

    int layerCount = getJSONArrayLength(sceneJson, "layers");
    td_linkedList *layers = malloc(sizeof(td_linkedList) * layerCount);
    td_hashMap layerIndexes = createHashMap(layerCount);
    td_hashMap behaviors = createHashMap(10);
    td_linkedList immutableColliders = createLinkedList();
    td_linkedList mutableColliders = createLinkedList();

    struct callbackData layerData = {
        0,
        layers,
        layerIndexes,
        game,
        behaviors,
        NULL,
        mutableColliders,
        immutableColliders
    };

    jsonArrayForEach(sceneJson, "layers", layerCallback, &layerData);
    jsonArrayForEach(sceneJson, "entities", entityCallback, &layerData);
    jsonArrayForEach(sceneJson, "tiled_regions", tilesetCallback, &layerData);

    td_linkedList entities = createLinkedList();

    for(int i = 0; i < layerCount; i++) {
        appendList(entities, layers[i]);
        destroyLinkedList(layers[i]);
    }

    // TODO: Find a more graceful way of doing this
    dangerouslyAddFreeFunc(entities, destroyEntity);

    scene -> entities = entities;
    scene -> behaviors = behaviors;
    scene -> immutableColliders = immutableColliders;
    scene -> mutableColliders = mutableColliders;

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}

void executeBehaviorCallback(void *entryData, void *callbackData, char *key) {
    lua_State *state = (lua_State*) callbackData;
    executeScript(state, (td_script) entryData);
}

void executeUpdateBehaviors(lua_State *state, td_scene scene) {
    td_linkedList updateBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, "on_update");

    if(!updateBehaviors) {
        return;
    }
    listForEach(updateBehaviors, executeBehaviorCallback, state);
}

void immutableColliderCallback(void *entryData, void *callbackData, char *key) {
    td_boxCollider mutableCollider = (td_boxCollider) callbackData;
    td_boxCollider immutableCollider = (td_boxCollider) entryData;
    checkCollision(mutableCollider, immutableCollider);
}

void mutableColliderCallback(void *entryData, void *callbackData, char *key) {
    td_linkedList immutableColiders = (td_linkedList) callbackData;
    listForEach(immutableColiders, immutableColliderCallback, entryData);
}

void resolveCollisions(td_scene scene) {
    listForEach(scene -> mutableColliders, mutableColliderCallback, scene -> immutableColliders);
}

void executeEventBehaviors(lua_State *state, td_scene scene, td_hashMap keymap, SDL_Event e) {
    // Handle keyboard events
    // TODO: This should probably be moved to the keyboard events module
    switch (e.type) {
        case SDL_KEYDOWN: {
            if(e.key.repeat != 0) {
                break;
            }
            const char* keyname = keySymToString(e.key.keysym.sym);
            td_keymap map = (td_keymap) getFromHashMap(keymap, (char *) keyname);
            if(!map) {
                break;
            }
            char *action = map -> down;
            if(!action) {
                break;
            }
            td_linkedList keyBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, action);
            listForEach(keyBehaviors, executeBehaviorCallback, state);
            break;
        }
        case SDL_KEYUP: {
            if(e.key.repeat != 0) {
                break;
            }
            const char* keyname = keySymToString(e.key.keysym.sym);
            td_keymap map = (td_keymap) getFromHashMap(keymap, (char *) keyname);
            if(!map) {
                break;
            }
            char *action = map -> up;
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
}

td_entity getEntityByID(td_scene scene, char *ID) {
    return getFromList(scene -> entities, ID);
}

td_linkedList getEntities(td_scene scene) {
    return scene -> entities;
}

void destroyScene(td_scene scene) {
    destroyLinkedList(scene -> entities);
    destroyLinkedList(scene -> mutableColliders);
    destroyLinkedList(scene -> immutableColliders);
    destroyHashMap(scene -> behaviors);
    free(scene);
}
