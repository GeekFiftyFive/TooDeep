#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "entity.h"
#include "../../Physics/physics.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../../IO/logger.h"

struct td_entity {
    char *ID;
    td_renderable renderable;
    td_physicsObject physicsObject;
    td_linkedList collisionHulls;
    td_tuple posDelta; // TODO: Find a better way of tracking this
    td_animation animation;
    td_hashMap animations;
    td_linkedList renderables;
    td_entityFlip flip;
};

td_entity createEntity(char *ID, td_renderable renderable) {
    td_entity entity = malloc(sizeof(struct td_entity));
    entity -> ID = ID;
    entity -> renderable = renderable;
    entity -> physicsObject = createPhysicsObject();
    entity -> collisionHulls = createLinkedList();
    entity -> posDelta = (td_tuple) { 0.0, 0.0 };
    entity -> animation = NULL;
    entity -> animations = createHashMap(10);
    entity -> flip = TD_NO_FLIP;
    entity -> renderables = createLinkedList();
    return entity;
}

void moveCollider(void *entryData, void *callbackData, char *key) {
    td_boxCollider collider = (td_boxCollider) entryData;
    td_tuple *delta = (td_tuple*) callbackData;
    td_tuple position = getBoxColliderPosition(collider);
    position = addTuple(position, *delta);
    setBoxColliderPosition(collider, position);
}

void playAnimation(td_entity entity, td_animation animation) {
    setAnimationPlaying(entity -> animation, false);
    setAnimationPlaying(animation, true);
    entity -> animation = animation;
    td_tuple renderablePosition = getRenderablePosition(entity -> renderable);
    entity -> renderable = getRenderableFromAnimation(animation);
    setRenderablePosition(entity -> renderable, renderablePosition);
}

td_animation getAnimationFromEntity(td_entity entity, const char *name) {
    return getFromHashMap(entity -> animations, (char *) name);
}

void setAnimation(td_entity entity, td_animation animation) {
    entity -> animation = animation;
}

static SDL_RendererFlip flipConversion(td_entityFlip flip) {
    switch(flip) {
        case TD_NO_FLIP:
            return SDL_FLIP_NONE;
        case TD_HORIZONTAL_FLIP:
            return SDL_FLIP_HORIZONTAL;
        case TD_VERTICAL_FLIP:
            return SDL_FLIP_VERTICAL;
    }

    return SDL_FLIP_NONE;
}

void addAnimation(td_entity entity, td_animation animation, char *name) {
    td_renderable renderable = getRenderableFromAnimation(animation); 
    setRenderableFlip(renderable, flipConversion(entity -> flip));
    append(entity -> renderables, renderable, name);
    insertIntoHashMap(entity -> animations, name, animation, NULL);
}

void setEntityGravityAcceleration(td_entity entity, float gravityAcceleration) {
    setPhysicsObjectGravityAcceleration(entity -> physicsObject, gravityAcceleration);
}

void setEntityPosition(td_entity entity, td_tuple position) {
    td_tuple currentPosition = getPhysicsObjectPosition(entity -> physicsObject);
    setPhysicsObjectPosition(entity -> physicsObject, position);
    setRenderablePosition(entity -> renderable, position);
    td_tuple delta = subtractTuple(position, currentPosition);
    listForEach(entity -> collisionHulls, moveCollider, &delta);
}

td_tuple getEntityPosition(td_entity entity) {
    return getPhysicsObjectPosition(entity -> physicsObject);
}

void flipCallback(void *entryData, void *callbackData, char *key) {
    SDL_RendererFlip flip = *((SDL_RendererFlip*) callbackData);
    td_renderable renderable = (td_renderable) entryData;
    setRenderableFlip(renderable, flip);
}

void setEntityFlip(td_entity entity, td_entityFlip flip) {
    entity -> flip = flip;
    SDL_RendererFlip sdlFlip = flipConversion(flip);
    setRenderableFlip(entity -> renderable, sdlFlip);
    listForEach(entity -> renderables, flipCallback, &sdlFlip);
}

void setEntityVelocity(td_entity entity, td_tuple position) {
    setPhysicsObjectVelocity(entity -> physicsObject, position);
}

td_tuple getEntityVelocity(td_entity entity) {
    return getPhysicsObjectVelocity(entity -> physicsObject);
}

td_renderable getRenderable(td_entity entity) {
    return entity -> renderable;
}

void enableEntityGravity(td_entity entity, bool gravity) {
    enableGravity(entity -> physicsObject, gravity);
}

char *getEntityID(td_entity entity) {
    return entity -> ID;
}

void applyForceToEntity(td_entity entity, td_tuple force) {
    applyForceToPhysicsObject(entity -> physicsObject, force);
}

void addCollisionHull(td_entity entity, td_boxCollider collider, char *key) {
    append(entity -> collisionHulls, collider, key);
}

td_tuple getPositionDelta(td_entity entity) {
    return entity -> posDelta;
}

td_boxCollider getCollisionHull(td_entity entity, char *name) {
    return getFromList(entity -> collisionHulls, name);
}

void entityPhysicsUpdate(void *data, void *callbackData, char *key) {
    td_entity entity = (td_entity) data;
    int delta = *((int*) callbackData);
    td_tuple currentPosition = getPhysicsObjectPosition(entity -> physicsObject);
    td_tuple position = updatePhysicsObject(entity -> physicsObject, delta);
    setRenderablePosition(entity -> renderable, position);
    td_tuple posDelta = subtractTuple(position, currentPosition);
    entity -> posDelta = posDelta;
    listForEach(entity -> collisionHulls, moveCollider, &posDelta);
}

void destroyEntity(td_entity entity) {
    destroyRenderable(entity -> renderable);
    destroyPhysicsObject(entity -> physicsObject);
    destroyLinkedList(entity -> collisionHulls);
    destroyLinkedList(entity -> renderables);
    destroyHashMap(entity -> animations);
    free(entity);
}
