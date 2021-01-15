#ifndef TD_PHYSICS
#define TD_PHYSICS

#include <stdbool.h>
#include "../DataStructures/Tuple/tuple.h"

typedef struct td_physicsObject *td_physicsObject;

td_physicsObject createPhysicsObject();

void setPhysicsObjectPosition(td_physicsObject, td_tuple);

td_tuple getPhysicsObjectPosition(td_physicsObject);

void setPhysicsObjectVelocity(td_physicsObject, td_tuple);

td_tuple getPhysicsObjectVelocity(td_physicsObject);

void enableGravity(td_physicsObject, bool);

void applyForceToPhysicsObject(td_physicsObject, td_tuple);

td_tuple updatePhysicsObject(td_physicsObject, int);

void destroyPhysicsObject(td_physicsObject);

#endif