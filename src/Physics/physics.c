#include <stdlib.h>
#include "physics.h"
#include "../IO/logger.h"

// TODO: Allow this to be configured
#define GRAVITY 0.00

struct td_physicsObject {
    td_tuple position;
    td_tuple velocity;
};

td_physicsObject createPhysicsObject() {
    td_physicsObject physicsObject = malloc(sizeof(struct td_physicsObject));
    physicsObject -> position = (td_tuple) { 0, 0 };
    physicsObject -> velocity = (td_tuple) { 0, 0 };
    return physicsObject;
}

td_tuple getPhysicsObjectPosition(td_physicsObject physicsObject) {
    return physicsObject -> position;
}

void setPhysicsObjectPosition(td_physicsObject physicsObject, td_tuple position) {
    physicsObject -> position = position;
}

void setPhysicsObjectVelocity(td_physicsObject physicsObject, td_tuple velocity) {
    physicsObject -> velocity = velocity;
}

td_tuple getPhysicsObjectVelocity(td_physicsObject physicsObject) {
    return physicsObject -> velocity;
}

td_tuple updatePhysicsObject(td_physicsObject physicsObject, int delta) {
    td_tuple posDelta = multiplyTuple(delta, physicsObject -> velocity);
    physicsObject -> position = addTuple(physicsObject -> position, posDelta);
    physicsObject -> velocity = addTuple(
        physicsObject -> velocity,
        (td_tuple) { 0, GRAVITY * -delta }
    );
    return physicsObject -> position;
}

void destroyPhysicsObject(td_physicsObject physicsObject) {
    free(physicsObject);
}
