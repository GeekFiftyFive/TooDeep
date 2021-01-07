#ifndef TD_SCENE
#define TD_SCENE

#include "../../IO/gameLoader.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../Entity/entity.h"
#include <SDL2/SDL.h>

typedef struct td_scene *td_scene;

td_scene buildScene(td_game, char *);

void executeBehaviors(lua_State *, td_scene, td_hashMap, SDL_Event);

td_entity getEntityByID(td_scene, char *);

td_linkedList getEntities(td_scene);

void destroyScene(td_scene);

#endif