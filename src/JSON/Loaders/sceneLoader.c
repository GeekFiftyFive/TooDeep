#include "sceneLoader.h"
#include "stateMachineLoader.h"
#include "../../IO/resourceLoader.h"
#include "../../IO/logger.h"
#include "../../Events/keyboardEvents.h"
#include "../../DataStructures/Box/box.h"
#include "../../Physics/boxCollision.h"
#include "../../Renderer/animation.h"
#include "../../State/StateMachine/stateMachine.h"
#include "../../Scripting/luaFunctions.h"

#define ASSET_DIR "/assets/"
#define SCRIPT_DIR "/scripts/"

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

struct addBehaviourCallbackData {
    char *fieldName;
    struct callbackData *data;
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
            val.booleanVal = getJSONBool(json, NULL, NULL);
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

void loadBehaviour(td_json json, struct callbackData *data, char *fieldName) {
    char *scriptName = getJSONString(json, "script", NULL);
    char *fullScriptName = malloc(strlen(scriptName) + strlen(SCRIPT_DIR) + 1);
    sprintf(fullScriptName, "%s%s", SCRIPT_DIR, scriptName);
    char *scriptContent = loadPlaintextResource(getResourceLoader(data -> game), fullScriptName);
    td_script script = createScript(scriptContent);
    jsonObjectForEach(json, "variables", registerVariableCallback, script);
    td_scriptVal entityIDVal;
    entityIDVal.stringVal = data -> entityID;
    registerVariable(script, "entityID", STRING, entityIDVal);

    td_linkedList actionList = getFromHashMap(data -> behaviors, fieldName);

    if(!actionList) {
        actionList = createLinkedList();
        insertIntoHashMap(data -> behaviors, fieldName, actionList, destroyLinkedList);    
    }

    appendWithFree(actionList, script, fullScriptName, destroyScript);
}

void loadBehaviourCallback(td_json json, void *data) {
    struct addBehaviourCallbackData *callbackData = (struct addBehaviourCallbackData *) data;
    loadBehaviour(json, callbackData -> data, callbackData -> fieldName);
}

void behaviourCallback(td_json json, void *data) {
    char *fieldName = getFieldName(json);
    struct callbackData *dataCast = (struct callbackData*) data;

    if(isJSONFieldArray(json, NULL)) {
        struct addBehaviourCallbackData behaviourCallbackData = { fieldName, dataCast };
        jsonArrayForEach(json, NULL, loadBehaviourCallback, &behaviourCallbackData);
    } else {
        loadBehaviour(json, dataCast, fieldName);
    }
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

    setParentEntityID(collider, getEntityID(callbackData -> entity));

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
    td_stateMachine machine = NULL;

    char *animationName = NULL;
    char *assetName = getJSONString(entityJSON, "start_look.asset", NULL);

    if(!assetName) {
        // Get animation name
        animationName = getJSONString(entityJSON, "start_look.animation", NULL);
        if(!animationName) {
            char *stateMachineName = getJSONString(entityJSON, "start_look.animation_state_machine", NULL);
            td_json stateMachineJSON = getStateMachine(dataCast -> game, stateMachineName);
            machine = loadStateMachineFromJSON(stateMachineJSON);
            animationName = getCurrentStateId(machine);
        }
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
    char *entityID = getJSONString(json, "entity_id", NULL);
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
    if(machine) {
        addAnimationStateMachine(entity, machine);
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

td_scene loadSceneFromJSON(td_json json, td_game game) {
    td_scene scene = createScene();

    int layerCount = getJSONArrayLength(json, "layers");
    td_linkedList *layers = malloc(sizeof(td_linkedList) * layerCount);
    td_hashMap layerIndexes = createHashMap(layerCount);

    td_hashMap behaviors = getBehaviors(scene);
    td_linkedList immutableColliders = getWorldColliders(scene);
    td_linkedList mutableColliders = getEntityColliders(scene);
    td_linkedList cameras = getCameras(scene);
    td_linkedList animations = getAnimations(scene);

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

    jsonArrayForEach(json, "layers", layerCallback, &layerData);
    jsonArrayForEach(json, "entities", entityCallback, &layerData);
    jsonArrayForEach(json, "tiled_regions", tilesetCallback, &layerData);
    jsonArrayForEach(json, "cameras", addCameraCallback, &cameraCallbackData);

    td_linkedList entities = getEntities(scene);

    for(int i = 0; i < layerCount; i++) {
        appendList(entities, layers[i]);
        destroyLinkedList(layers[i]);
    }

    // TODO: Find a more graceful way of doing this
    dangerouslyAddFreeFunc(entities, destroyEntity);

    destroyHashMap(layerIndexes);

    free(layers);

    return scene;
}
