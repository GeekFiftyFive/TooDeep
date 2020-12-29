#ifndef TD_SCENE
#define TD_SCENE

#include "../../IO/gameLoader.h"
#include "../../DataStructures/LinkedList/linkedList.h"

typedef struct td_scene *td_scene;

td_scene buildScene(td_game, char *);

td_linkedList getEntities(td_scene scene);

void destroyScene(td_scene);

#endif