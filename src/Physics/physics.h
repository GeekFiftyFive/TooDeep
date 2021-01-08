#ifndef TD_PHYSICS
#define TD_PHYSICS

#include "../DataStructures/Tuple/tuple.h"

typedef struct td_physicsObject *td_physicsObject;

td_physicsObject createPhysicsObject();

void setPhysicsObjectPosition(td_physicsObject, td_tuple);

td_tuple getPhysicsObjectPosition(td_physicsObject);

void destroyPhysicsObject(td_physicsObject);

#endif