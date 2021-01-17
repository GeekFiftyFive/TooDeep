#include "camera.h"
#include <stdlib.h>

struct td_camera {
    float zoom;
    td_tuple position;
};

td_camera createCamera() {
    td_camera camera = malloc(sizeof(struct td_camera));

    camera -> zoom = 1.0;
    camera -> position = (td_tuple) { 0.0, 0.0 };

    return camera;
}

float getCameraZoom(td_camera camera) {
    return camera -> zoom;
}

void setCameraZoom(td_camera camera, float zoom) {
    camera -> zoom = zoom;    
}

td_tuple getCameraPosition(td_camera camera) {
    return camera -> position;
}

void setCameraPosition(td_camera camera, td_tuple position) {
    camera -> position = position;
}

void destroyCamera(td_camera camera) {
    free(camera);
}
