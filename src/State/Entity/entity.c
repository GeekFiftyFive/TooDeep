#include <stdlib.h>
#include "entity.h"

struct td_entity {
    td_renderable renderable;
};

td_entity createEntity(td_renderable renderable) {
    td_entity entity = malloc(sizeof(struct td_entity));
    entity -> renderable = renderable;
    return entity;
}

td_renderable getRenderable(td_entity entity) {
    return entity -> renderable;
}

void destroyEntity(td_entity entity) {
    //destroyRenderable(entity -> renderable);
    //free(entity);
}
