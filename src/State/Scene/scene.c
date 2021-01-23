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
#include "../../Renderer/animation.h"
#include "../../Scripting/luaCallback.h"

// TODO: This need A LOT of cleaning up!

#define ASSET_DIR "/assets/"
#define SCRIPT_DIR "/scripts/"

struct td_scene {
    td_linkedList entities;
    td_hashMap behaviors;
    td_linkedList mutableColliders;
    td_linkedList immutableColliders;
    td_linkedList cameras;
    td_linkedList animations;
    td_linkedList timeouts;
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
    td_linkedList animations;
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

struct addCameraCallbackData {
    td_linkedList cameras;
    td_renderer renderer;
};

struct checkWorldCollisionsCallbackData {
    td_boxCollider collider;
    bool collided;
};

struct executeTimeoutCallbackData {
    td_linkedList toRemove;
    lua_State *state;
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
    td_scriptValType variableType;
    td_scriptVal val;

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
    td_scriptVal entityIDVal;
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

    td_tuple hullPosition = { collision.hull.x, collision.hull.y };
    td_tuple topLeft = subtractTuple(hullPosition, getPositionDelta(entity));
    td_tuple bottomRight = { topLeft.x + collision.hull.w, topLeft.y - collision.hull.h };
    td_box collidingHull = collision.collidingHull;

    td_tuple delta = { 0.0, 0.0 };
    td_tuple newVelocity = getEntityVelocity(entity);

    if(bottomRight.y >= collidingHull.y || topLeft.y <= collidingHull.y - collidingHull.h) {
        // Collided in the y axis
        delta.y = -collision.intrusion.y;
        newVelocity.y = 0.0;
    } else {
        // Collided in the x axis
        delta.x = -collision.intrusion.x;
    }

    setEntityPosition(entity, addTuple(position, delta));
    setEntityVelocity(entity, newVelocity);
}

void addCollisionHullCallback(td_json json, void *data) {
    struct addCollisionHullCallbackData *callbackData = (struct addCollisionHullCallbackData *) data;
    td_tuple entityStartPosition = getEntityPosition(callbackData -> entity);
    float x = (float) getJSONDouble(json, "x", NULL);
    float y = (float) getJSONDouble(json, "y", NULL);
    float w = (float) getJSONDouble(json, "w", NULL);
    float h = (float) getJSONDouble(json, "h", NULL);
    bool intangible = getJSONBool(json, "intangible", NULL);
    char *name = getJSONString(json, "name", NULL);
    td_box hull = {
        entityStartPosition.x + x,
        entityStartPosition.y + y,
        w,
        h
    };
    td_boxCollider collider = createBoxCollider(hull);
    if(!intangible) {
        registerBoxColliderCallback(collider, handleCollision);
        registerBoxColliderCallbackData(collider, callbackData -> entity);
    }

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

    td_renderable renderable;
    td_animation animation = NULL;

    char *animationName = NULL;
    char *assetName = getJSONString(json, "start_look.asset", NULL);

    if(!assetName) {
        // Get animation name
        animationName = getJSONString(entityJSON, "start_look.animation", NULL);
        td_json animationJSON = getAnimation(dataCast -> game, animationName);
        char *name = getJSONString(animationJSON, "name", NULL);

        animation = createAnimationFromJson(
            animationJSON,
            getResourceLoader(dataCast -> game),
            getRenderer(dataCast -> game)
        );

        appendWithFree(dataCast -> animations, animation, name, destroyAnimation);
        renderable = getRenderableFromAnimation(animation);
    } else {
        char *fullAssetName = malloc(strlen(assetName) + strlen(ASSET_DIR) + 1);
        sprintf(fullAssetName, "%s%s", ASSET_DIR, assetName);

        // Load asset using asset name and create renderable
        SDL_Surface *startLook =  loadSurfaceResource(getResourceLoader(dataCast -> game), fullAssetName);
        free(fullAssetName);
        renderable = createRenderableFromSurface(getRenderer(dataCast -> game), startLook);
    }

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
    if(animation) {
        addAnimation(entity, animation, animationName);
        setAnimation(entity, animation);
    }
    setEntityPosition(entity, pos);
    enableEntityGravity(entity, gravityEnabled);
    if(jsonFieldExists(entityJSON, "physics.gravity_acceleration")) {
        float gravityAccleration = (float) getJSONDouble(entityJSON, "physics.gravity_acceleration", NULL);
        setEntityGravityAcceleration(entity, gravityAccleration);
    }
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

void addCameraCallback(td_json json, void *data) {
    td_linkedList cameras = ((struct addCameraCallbackData*) data) -> cameras;
    td_renderer renderer = ((struct addCameraCallbackData*) data) -> renderer;
    td_camera camera = createCamera();

    // Get camera info
    float x = getJSONDouble(json, "start_pos.x", NULL);
    float y = getJSONDouble(json, "start_pos.y", NULL);
    float zoom = getJSONDouble(json, "zoom", NULL);
    bool defaultCamera = getJSONBool(json, "default_camera", NULL);
    char *name = getJSONString(json, "name", NULL);

    // Setup camera
    setCameraPosition(camera, (td_tuple) { x, y });
    setCameraZoom(camera, zoom);

    if(defaultCamera) {
        setCurrentCamera(renderer, camera);
    }

    appendWithFree(cameras, camera, name, destroyCamera);
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
    td_linkedList cameras = createLinkedList();
    td_linkedList animations = createLinkedList();

    struct callbackData layerData = {
        0,
        layers,
        layerIndexes,
        game,
        behaviors,
        NULL,
        mutableColliders,
        immutableColliders,
        animations
    };

    struct addCameraCallbackData cameraCallbackData = { cameras, getRenderer(game) };

    jsonArrayForEach(sceneJson, "layers", layerCallback, &layerData);
    jsonArrayForEach(sceneJson, "entities", entityCallback, &layerData);
    jsonArrayForEach(sceneJson, "tiled_regions", tilesetCallback, &layerData);
    jsonArrayForEach(sceneJson, "cameras", addCameraCallback, &cameraCallbackData);

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
    scene -> cameras = cameras;
    scene -> animations = animations;
    scene -> timeouts = createLinkedList();

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}

void registerTimeout(td_scene scene, int reference, float timeout) {
    td_luaCallback callback = createLuaCallback(reference, SDL_GetTicks(), timeout);
    append(scene -> timeouts, callback, NULL);
}

void executeTimeoutCallback(void *entryData, void *callbackData, char *key) {
    td_luaCallback callback = (td_luaCallback) entryData;
    lua_State *state = ((struct executeTimeoutCallbackData*) callbackData) -> state;
    td_linkedList toRemove = ((struct executeTimeoutCallbackData*) callbackData) -> toRemove;
    if(shouldTriggerLuaCallback(callback, SDL_GetTicks())) {
        executeCallback(state, getLuaCallbackReference(callback));
        appendWithFree(toRemove, entryData, NULL, destroyLuaCallback);
    }
}

void removeTimeoutCallback(void *entryData, void *callbackData, char *key) {
    td_linkedList timeouts = (td_linkedList) callbackData;
    removeFromListMatchPointer(timeouts, entryData);
}

void executeTimeouts(lua_State *state, td_scene scene) {
    td_linkedList toRemove = createLinkedList();
    struct executeTimeoutCallbackData callbackData = { toRemove, state };
    listForEach(scene -> timeouts, executeTimeoutCallback, &callbackData);
    listForEach(toRemove, removeTimeoutCallback, scene -> timeouts);
    destroyLinkedList(toRemove);
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
    td_boxCollider mutableCollider = ((struct checkWorldCollisionsCallbackData*) callbackData) -> collider;
    td_boxCollider immutableCollider = (td_boxCollider) entryData;
    bool collided = checkCollision(mutableCollider, immutableCollider);
    if(collided) {
        ((struct checkWorldCollisionsCallbackData*) callbackData) -> collided = true;
    }
}

bool checkWorldCollisions(td_boxCollider collider, td_linkedList colliders) {
    struct checkWorldCollisionsCallbackData callbackData = { collider, false };
    listForEach(colliders, immutableColliderCallback, &callbackData);
    return callbackData.collided;
}

void mutableColliderCallback(void *entryData, void *callbackData, char *key) {
    checkWorldCollisions((td_boxCollider) entryData, (td_linkedList) callbackData);
}

void resolveCollisions(td_scene scene) {
    listForEach(scene -> mutableColliders, mutableColliderCallback, scene -> immutableColliders);
}

void forAllColliders(td_scene scene, void *callbackFunction, void *callbackData) {
    listForEach(scene -> immutableColliders, callbackFunction, callbackData);
    listForEach(scene -> mutableColliders, callbackFunction, callbackData);
}

void iterateAnimationsCallback(void *entryData, void *callbackData, char *key) {
    td_animation animation = (td_animation) entryData;
    iterateAnimation(animation);
}

void iterateAnimations(td_scene scene) {
    listForEach(scene -> animations, iterateAnimationsCallback, NULL);
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
            if(!keyBehaviors) {
                break;
            }
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
            if(!keyBehaviors) {
                break;
            }
            listForEach(keyBehaviors, executeBehaviorCallback, state);
            break;
        }
        default:
            break;
    }
}

td_linkedList getWorldColliders(td_scene scene) {
    return scene -> immutableColliders;
}

td_entity getEntityByID(td_scene scene, char *ID) {
    return getFromList(scene -> entities, ID);
}

td_camera getCameraByID(td_scene scene, char *ID) {
    return getFromList(scene -> cameras, ID);
}

td_linkedList getEntities(td_scene scene) {
    return scene -> entities;
}

void addAnimationToScene(td_scene scene, td_animation animation, char *name) {
    appendWithFree(scene -> animations, animation, name, destroyAnimation);
}

void destroyScene(td_scene scene) {
    destroyLinkedList(scene -> entities);
    destroyLinkedList(scene -> mutableColliders);
    destroyLinkedList(scene -> immutableColliders);
    destroyLinkedList(scene -> cameras);
    destroyLinkedList(scene -> animations);
    destroyLinkedList(scene -> timeouts);
    destroyHashMap(scene -> behaviors);
    free(scene);
}
