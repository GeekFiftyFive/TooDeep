#ifndef TD_SCENE
#define TD_SCENE

#include "../../IO/gameLoader.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../Entity/entity.h"

typedef struct td_scene *td_scene;

td_scene buildScene(td_game, char *);

void executeBehaviors(lua_State *state, td_scene scene);

void setCurrentScene(td_scene);

td_scene getCurrentScene();

td_entity getEntityByID(td_scene, char *);

td_linkedList getEntities(td_scene);

void destroyScene(td_scene);

#endif