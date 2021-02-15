#ifndef ENTITY
#define ENTITY

#include "../EntityState/entityState.h"
#include "../../Renderer/renderer.h"
#include "../../Physics/boxCollision.h"
#include "../../Renderer/animation.h"
#include "../StateMachine/stateMachine.h"

typedef enum {TD_NO_FLIP, TD_HORIZONTAL_FLIP, TD_VERTICAL_FLIP} td_entityFlip;

typedef struct td_entity *td_entity;

td_entity createEntity(char *, td_renderable);

td_entityState getEntityState(td_entity);

void playAnimation(td_entity, td_animation);

td_animation getAnimationFromEntity(td_entity, const char *);

void setAnimation(td_entity, td_animation);

void addAnimation(td_entity, td_animation, char *);

void addAnimationStateMachine(td_entity, td_stateMachine);

td_stateMachine getAnimationStateMachine(td_entity);

float getEntityGravityAccleration(td_entity);

void setEntityGravityAcceleration(td_entity, float);

void setEntityTerminalVelocity(td_entity, float);

void setEntityPosition(td_entity, td_tuple);

void setEntityFlip(td_entity, td_entityFlip);

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

td_tuple getEntityBoxColliderPosition(td_entity, char *);

void setEntityBoxColliderPosition(td_entity, char *, td_tuple);

td_tuple getEntityBoxColliderDimensions(td_entity, char *);

void setEntityBoxColliderDimensions(td_entity, char *, td_tuple);

void flipEntityColliderHorizontally(td_entity, char *, td_entityFlip);

void entityPhysicsUpdate(void *, void *, char *);

void destroyEntity(td_entity);

#endif
