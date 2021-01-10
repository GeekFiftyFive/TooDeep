#ifndef ENTITY
#define ENTITY

#include "../../Renderer/renderer.h"

typedef struct td_entity *td_entity;

td_entity createEntity(char *, td_renderable);

void setEntityPosition(td_entity, td_tuple);

void setEntityVelocity(td_entity, td_tuple);

td_tuple getEntityPosition(td_entity);

td_tuple getEntityVelocity(td_entity);

td_renderable getRenderable(td_entity);

char *getEntityID(td_entity);

void entityPhysicsUpdate(void *data, void *callbackData, char *key);

void destroyEntity(td_entity);

#endif
