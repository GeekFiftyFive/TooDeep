#include <stdlib.h>
#include "entity.h"

struct td_entity {
    td_renderable renderable;
    td_tuple position;
    td_tuple velocity;
};

td_entity createEntity(td_renderable renderable, td_tuple position, td_tuple velocity) {
    return NULL;
}

void destroyEntity(td_entity entity) {
    // TODO: Implement
}
