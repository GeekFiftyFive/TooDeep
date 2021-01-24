#include <stdlib.h>
#include "camera.h"
#include "../Physics/physics.h"

struct td_camera {
    float zoom;
    td_physicsObject physicsObject;
};

td_camera createCamera() {
    td_camera camera = malloc(sizeof(struct td_camera));

    camera -> zoom = 1.0;
    camera -> physicsObject = createPhysicsObject();

    return camera;
}

float getCameraZoom(td_camera camera) {
    return camera -> zoom;
}

void setCameraZoom(td_camera camera, float zoom) {
    camera -> zoom = zoom;    
}

td_tuple getCameraPosition(td_camera camera) {
    return getPhysicsObjectPosition(camera -> physicsObject);
}

void setCameraPosition(td_camera camera, td_tuple position) {
    setPhysicsObjectPosition(camera -> physicsObject, position);
}

td_tuple getCameraVelocity(td_camera camera) {
    return getPhysicsObjectVelocity(camera -> physicsObject);
}

void setCameraVelocity(td_camera camera, td_tuple velocity) {
    setPhysicsObjectVelocity(camera -> physicsObject, velocity);
}

void updateCameraPosition(td_camera camera, int delta) {
    updatePhysicsObject(camera -> physicsObject, delta);
}

void destroyCamera(td_camera camera) {
    free(camera);
}
