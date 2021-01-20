#ifndef ENTITY
#define ENTITY

#include "../../Renderer/renderer.h"
#include "../../Physics/boxCollision.h"
#include "../../Renderer/animation.h"

typedef struct td_entity *td_entity;

td_entity createEntity(char *, td_renderable);

void playAnimation(td_entity, td_animation);

td_animation getAnimationFromEntity(td_entity, const char *);

void setAnimation(td_entity, td_animation);

void addAnimation(td_entity, td_animation, char *);

void setEntityPosition(td_entity, td_tuple);

void setEntityVelocity(td_entity, td_tuple);

td_tuple getEntityPosition(td_entity);

td_tuple getEntityVelocity(td_entity);

td_renderable getRenderable(td_entity);

void enableEntityGravity(td_entity, bool);

char *getEntityID(td_entity);

void applyForceToEntity(td_entity, td_tuple);

void addCollisionHull(td_entity, td_boxCollider, char *);

td_tuple getPositionDelta(td_entity);

td_boxCollider getCollisionHull(td_entity, char *);

void entityPhysicsUpdate(void *, void *, char *);

void destroyEntity(td_entity);

#endif
