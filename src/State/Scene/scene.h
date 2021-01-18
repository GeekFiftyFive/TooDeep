#ifndef TD_SCENE
#define TD_SCENE

#include <SDL2/SDL.h>
#include "../Entity/entity.h"
#include "../../IO/gameLoader.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../../Renderer/camera.h"

typedef struct td_scene *td_scene;

td_scene buildScene(td_game, char *);

void physicsUpdate(td_scene, int);

void executeUpdateBehaviors(lua_State *, td_scene);

void iterateAnimations(td_scene);

void resolveCollisions(td_scene);

void executeEventBehaviors(lua_State *, td_scene, td_hashMap, SDL_Event);

td_entity getEntityByID(td_scene, char *);

td_camera getCameraByID(td_scene, char *);

td_linkedList getEntities(td_scene);

void destroyScene(td_scene);

#endif