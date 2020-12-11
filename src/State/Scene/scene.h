#ifndef TD_SCENE
#define TD_SCENE

#include "../../IO/gameLoader.h"

typedef struct td_scene *td_scene;

td_scene buildScene(td_game, char *);

void destroyScene(td_scene);

#endif