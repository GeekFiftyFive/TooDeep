#include <stdlib.h>
#include "physics.h"

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

void destroyPhysicsObject(td_physicsObject physicsObject) {
    free(physicsObject);
}
