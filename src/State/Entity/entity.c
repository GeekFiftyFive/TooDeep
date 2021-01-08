#include <stdlib.h>
#include <stdio.h>
#include "entity.h"
#include "../../Physics/physics.h"

struct td_entity {
    char *ID;
    td_renderable renderable;
    td_physicsObject physicsObject;
};

td_entity createEntity(char *ID, td_renderable renderable) {
    td_entity entity = malloc(sizeof(struct td_entity));
    entity -> ID = ID;
    entity -> renderable = renderable;
    entity -> physicsObject = createPhysicsObject();
    return entity;
}

void setEntityPosition(td_entity entity, td_tuple position) {
    setPhysicsObjectPosition(entity -> physicsObject, position);
    setRenderablePosition(entity -> renderable, position);
}

td_tuple getEntityPosition(td_entity entity) {
    return getPhysicsObjectPosition(entity -> physicsObject);
}

td_renderable getRenderable(td_entity entity) {
    return entity -> renderable;
}

char *getEntityID(td_entity entity) {
    return entity -> ID;
}

void destroyEntity(td_entity entity) {
    destroyRenderable(entity -> renderable);
    destroyPhysicsObject(entity -> physicsObject);
    free(entity);
}
