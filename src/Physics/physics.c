#include <stdlib.h>
#include <math.h>
#include "physics.h"
#include "../IO/logger.h"

struct td_physicsObject {
    td_tuple position;
    td_tuple velocity;
    td_tuple force;
    bool gravity;
    float gravityAccel;
    float mass;
    float terminalVelocity;
};

td_physicsObject createPhysicsObject() {
    td_physicsObject physicsObject = malloc(sizeof(struct td_physicsObject));
    physicsObject -> position      = (td_tuple) { 0, 0 };
    physicsObject -> velocity      = (td_tuple) { 0, 0 };
    physicsObject -> force         = (td_tuple) { 0, 0 };
    physicsObject -> mass          = 1.0; // TODO: Make this configurable per object
    physicsObject -> gravity       = false;
    physicsObject -> gravityAccel  = 0.005;
    physicsObject -> terminalVelocity = INFINITY;
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

float getPhysicsObjectGravityAccleration(td_physicsObject physicsObject) {
    return physicsObject -> gravityAccel;
}

void setPhysicsObjectGravityAcceleration(td_physicsObject physicsObject, float gravity) {
    physicsObject -> gravityAccel = gravity;
}

void setPhysicsObjectTerminalVelocity(td_physicsObject physicsObject, float terminalVelocity) {
    physicsObject -> terminalVelocity = terminalVelocity;
}

void enableGravity(td_physicsObject physicsObject, bool gravity) {
    physicsObject -> gravity = gravity;
}

void applyForceToPhysicsObject(td_physicsObject physicsObject, td_tuple force) {
    physicsObject -> force = addTuple(physicsObject -> force, force);
}

td_tuple updatePhysicsObject(td_physicsObject physicsObject, int delta) {
    td_tuple posDelta = multiplyTuple(delta, physicsObject -> velocity);
    physicsObject -> position = addTuple(physicsObject -> position, posDelta);
    td_tuple acceleration = multiplyTuple(1.0 / physicsObject -> mass, physicsObject -> force);
    if(physicsObject -> gravity && -physicsObject -> velocity.y < physicsObject -> terminalVelocity) {
        acceleration = addTuple(acceleration, (td_tuple) { 0, -physicsObject -> gravityAccel });
    }
    physicsObject -> velocity = addTuple(physicsObject -> velocity, multiplyTuple(delta, acceleration));
    return physicsObject -> position;
}

void destroyPhysicsObject(td_physicsObject physicsObject) {
    free(physicsObject);
}
