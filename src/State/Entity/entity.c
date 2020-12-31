#include <stdlib.h>
#include <stdio.h>
#include "entity.h"

struct td_entity {
    char *ID;
    td_renderable renderable;
};

td_entity createEntity(char *ID, td_renderable renderable) {
    td_entity entity = malloc(sizeof(struct td_entity));
    entity -> ID = ID;
    entity -> renderable = renderable;
    return entity;
}

td_renderable getRenderable(td_entity entity) {
    return entity -> renderable;
}

char *getEntityID(td_entity entity) {
    return entity -> ID;
}

void destroyEntity(td_entity entity) {
    destroyRenderable(entity -> renderable);
    free(entity);
}
