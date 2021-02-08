#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "../../IO/logger.h"
#include "../../IO/resourceLoader.h"
#include "../../Events/keyboardEvents.h"
#include "../../DataStructures/Box/box.h"
#include "../../Physics/boxCollision.h"
#include "../../Renderer/animation.h"
#include "../../Scripting/luaCallback.h"
#include "../../Scripting/luaFunctions.h"
#include "../../State/StateMachine/stateMachine.h"
#include "../../JSON/Loaders/stateMachineLoader.h"

struct td_scene {
    td_linkedList entities;
    td_hashMap behaviors;
    td_linkedList mutableColliders;
    td_linkedList immutableColliders;
    td_linkedList cameras;
    td_linkedList animations;
    td_linkedList timeouts;
    td_linkedList firedEvents;
};

struct checkWorldCollisionsCallbackData {
    td_boxCollider collider;
    bool collided;
    bool executeCallbacks;
};

struct executeTimeoutCallbackData {
    td_linkedList toRemove;
    lua_State *state;
};

struct firedEvent {
    char *eventName;
    td_entity entity;
};

td_scene createScene() {
    td_scene scene = malloc(sizeof(struct td_scene));
    scene -> entities = createLinkedList();
    scene -> behaviors = createHashMap(10);
    scene -> immutableColliders = createLinkedList();
    scene -> mutableColliders = createLinkedList();
    scene -> cameras = createLinkedList();
    scene -> animations = createLinkedList();
    scene -> timeouts = createLinkedList();
    scene -> firedEvents = createLinkedList();
    return scene;
}

td_linkedList getEntities(td_scene scene) {
    return scene -> entities;
}

td_hashMap getBehaviors(td_scene scene) {
    return scene -> behaviors;
}

td_linkedList getCameras(td_scene scene) {
    return scene -> cameras;
}

td_linkedList getAnimations(td_scene scene) {
    return scene -> animations;
}

td_linkedList getTimeouts(td_scene scene) {
    return scene -> timeouts;
}

void pushFiredEvent(td_scene scene, char *eventName, td_entity entity) {
    char *nameCpy = malloc(strlen(eventName) + 1);
    strcpy(nameCpy, eventName);
    struct firedEvent *event = malloc(sizeof(struct firedEvent));
    event -> eventName = nameCpy;
    event -> entity = entity;
    appendWithFree(scene -> firedEvents, event, nameCpy, free);
}

void executeFiredEvents(lua_State *state, td_scene scene) {
    td_iterator eventsIterator = getIterator(scene -> firedEvents);

    struct firedEvent *event;

    while((event = (struct firedEvent*) iteratorNext(eventsIterator))) {
        td_linkedList behaviors = (td_linkedList) getFromHashMap(scene -> behaviors, event -> eventName);
        td_iterator behaviorIterator = getIterator(behaviors);
        td_script script;
        while((script = (td_script) iteratorNext(behaviorIterator))) {
            char *entityID = getScriptEntityID(script);
            if(strcmp(entityID, getEntityID(event -> entity)) == 0) {
                td_eventAttributes eventAttributes = createEventAttributes();
                executeScript(state, script, eventAttributes);
                destroyEventAttributes(eventAttributes);
            }
        }
        destroyIterator(behaviorIterator);
    }

    destroyIterator(eventsIterator);
    destroyLinkedList(scene -> firedEvents);
    scene -> firedEvents = createLinkedList();
}

static void cameraPhysicsUpdate(void *entryData, void *callbackData, char *key) {
    int delta = *((int*) callbackData);
    td_camera camera = (td_camera) entryData;
    updateCameraPosition(camera, delta);
}

void physicsUpdate(td_scene scene, int delta) {
    listForEach(scene -> entities, entityPhysicsUpdate, &delta);
    listForEach(scene -> cameras, cameraPhysicsUpdate, &delta);
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
    td_eventAttributes eventAttributes = createEventAttributes();
    executeScript(state, (td_script) entryData, eventAttributes);
    destroyEventAttributes(eventAttributes);
}

void executeUpdateBehaviors(lua_State *state, td_scene scene, int delta) {
    td_linkedList updateBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, "on_update");

    if(!updateBehaviors) {
        return;
    }

    td_eventAttributes eventAttributes = createEventAttributes();
    td_scriptVal val;
    val.intVal = delta;
    registerEventAttribute(eventAttributes, INT, val, "time_delta");

    td_iterator iterator = getIterator(updateBehaviors);
    td_script script;

    while ((script = iteratorNext(iterator))) {
        executeScript(state, script, eventAttributes);
    }

    destroyEventAttributes(eventAttributes);
    destroyIterator(iterator);
}

void immutableColliderCallback(void *entryData, void *callbackData, char *key) {
    td_boxCollider mutableCollider = ((struct checkWorldCollisionsCallbackData*) callbackData) -> collider;
    bool executeCallbacks = ((struct checkWorldCollisionsCallbackData*) callbackData) -> executeCallbacks;
    td_boxCollider immutableCollider = (td_boxCollider) entryData;
    if(mutableCollider == immutableCollider) {
        return;
    }
    bool collided = checkCollision(mutableCollider, immutableCollider, executeCallbacks);
    if(collided) {
        ((struct checkWorldCollisionsCallbackData*) callbackData) -> collided = true;
    }
}

td_linkedList getCollisions(td_boxCollider collider, td_linkedList colliders) {
    td_iterator iterator = getIterator(colliders);

    td_boxCollider collider2;
    td_linkedList collisions = createLinkedList();

    while((collider2 = iteratorNext(iterator))) {
        if(checkCollision(collider, collider2, false)) {
            append(collisions, collider2, NULL);
        }
    }

    destroyIterator(iterator);
    return collisions;
}

static bool checkCollisionsHelper(td_boxCollider collider, td_linkedList colliders, bool executeCallbacks) {
    struct checkWorldCollisionsCallbackData callbackData = { collider, false, executeCallbacks };
    listForEach(colliders, immutableColliderCallback, &callbackData);
    return callbackData.collided;
}

bool checkCollisionsWithoutCallbacks(td_boxCollider collider, td_linkedList colliders) {
    td_linkedList collisions = getCollisions(collider, colliders);
    int length = listLength(collisions);
    destroyLinkedList(collisions);
    return (bool) length;
}

bool checkCollisions(td_boxCollider collider, td_linkedList colliders) {
    return checkCollisionsHelper(collider, colliders, true);
}

void mutableColliderCallback(void *entryData, void *callbackData, char *key) {
    checkCollisions((td_boxCollider) entryData, (td_linkedList) callbackData);
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
    if(e.key.repeat != 0) {
        return;
    }
    const char* keyname = keySymToString(e.key.keysym.sym);
    td_keymap map = (td_keymap) getFromHashMap(keymap, (char *) keyname);
    if(!map) {
        return;
    }
    char *action;
    if(e.type == SDL_KEYDOWN) {
        action = map -> down;
    } else if(e.type == SDL_KEYUP) {
        action = map -> up;
    } else {
        return;
    }
    if(!action) {
        return;
    }
    td_linkedList keyBehaviors = (td_linkedList) getFromHashMap(scene -> behaviors, action);
    if(!keyBehaviors) {
        return;
    }
    
    td_eventAttributes eventAttributes = createEventAttributes();
    td_iterator iterator = getIterator(keyBehaviors);
    td_script script;

    while ((script = iteratorNext(iterator))) {
        executeScript(state, script, eventAttributes);
    }
    
    destroyEventAttributes(eventAttributes);
    destroyIterator(iterator);
}

td_linkedList getWorldColliders(td_scene scene) {
    return scene -> immutableColliders;
}

td_linkedList getEntityColliders(td_scene scene) {
    return scene -> mutableColliders;
}

td_entity getEntityByID(td_scene scene, char *ID) {
    return getFromList(scene -> entities, ID);
}

td_camera getCameraByID(td_scene scene, char *ID) {
    return getFromList(scene -> cameras, ID);
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
    destroyLinkedList(scene -> firedEvents);
    destroyHashMap(scene -> behaviors);
    free(scene);
}
