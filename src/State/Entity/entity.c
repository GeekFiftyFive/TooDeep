#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "entity.h"
#include "../../Physics/physics.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../IO/logger.h"

struct td_entity {
    char *ID;
    td_renderable renderable;
    td_physicsObject physicsObject;
    td_linkedList collisionHulls;
    td_tuple posDelta; // TODO: Find a better way of tracking this
};

td_entity createEntity(char *ID, td_renderable renderable) {
    td_entity entity = malloc(sizeof(struct td_entity));
    entity -> ID = ID;
    entity -> renderable = renderable;
    entity -> physicsObject = createPhysicsObject();
    entity -> collisionHulls = createLinkedList();
    entity -> posDelta = (td_tuple) { 0.0, 0.0 };
    return entity;
}

void moveCollider(void *entryData, void *callbackData, char *key) {
    td_boxCollider collider = (td_boxCollider) entryData;
    td_tuple *delta = (td_tuple*) callbackData;
    td_tuple position = getBoxColliderPosition(collider);
    position = addTuple(position, *delta);
    setBoxColliderPosition(collider, position);
}

void setEntityPosition(td_entity entity, td_tuple position) {
    td_tuple currentPosition = getPhysicsObjectPosition(entity -> physicsObject);
    setPhysicsObjectPosition(entity -> physicsObject, position);
    setRenderablePosition(entity -> renderable, position);
    td_tuple delta = subtractTuple(position, currentPosition);
    entity -> posDelta;
    listForEach(entity -> collisionHulls, moveCollider, &delta);
}

td_tuple getEntityPosition(td_entity entity) {
    return getPhysicsObjectPosition(entity -> physicsObject);
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
    free(entity);
}
