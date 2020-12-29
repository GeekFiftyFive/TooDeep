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

void destroyEntity(td_entity entity) {
    // TODO: Implement
}
