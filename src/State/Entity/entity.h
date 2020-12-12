#ifndef ENTITY
#define ENTITY

#include "../../Renderer/renderer.h"
#include "../../DataStructures/Tuple/tuple.h"

typedef struct td_entity *td_entity;

td_entity createEntity(td_renderable, td_tuple, td_tuple);

void destroyEntity(td_entity);

#endif
