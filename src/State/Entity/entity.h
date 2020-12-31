#ifndef ENTITY
#define ENTITY

#include "../../Renderer/renderer.h"

typedef struct td_entity *td_entity;

td_entity createEntity(char *, td_renderable);

td_renderable getRenderable(td_entity);

char *getEntityID(td_entity);

void destroyEntity(td_entity);

#endif
