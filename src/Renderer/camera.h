#ifndef TD_CAMERA
#define TD_CAMERA

#include "../DataStructures/Tuple/tuple.h"

typedef struct td_camera *td_camera;

td_camera createCamera();

float getCameraZoom(td_camera);

void setCameraZoom(td_camera, float);

td_tuple getCameraPosition(td_camera);

void setCameraPosition(td_camera, td_tuple);

void destroyCamera(td_camera);

#endif
