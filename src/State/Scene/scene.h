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

void forAllColliders(td_scene, void *, void *);

void executeUpdateBehaviors(lua_State *, td_scene, int);

void iterateAnimations(td_scene);

bool checkCollisions(td_boxCollider, td_linkedList);

void resolveCollisions(td_scene);

void registerTimeout(td_scene, int, float);

void executeTimeouts(lua_State *, td_scene);

void executeEventBehaviors(lua_State *, td_scene, td_hashMap, SDL_Event);

td_linkedList getWorldColliders(td_scene);

td_linkedList getEntityColliders(td_scene);

td_entity getEntityByID(td_scene, char *);

td_camera getCameraByID(td_scene, char *);

td_linkedList getEntities(td_scene);

void addAnimationToScene(td_scene, td_animation, char *);

void destroyScene(td_scene);

#endif