#ifndef TD_SCENE
#define TD_SCENE

#include <SDL2/SDL.h>
#include <lua.h>
#include "../Entity/entity.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../../Renderer/camera.h"

typedef struct td_scene *td_scene;

td_scene createScene();

td_linkedList getEntities(td_scene);

td_hashMap getBehaviors(td_scene);

td_linkedList getCameras(td_scene);

td_linkedList getAnimations(td_scene);

td_linkedList getTimeouts(td_scene);

void physicsUpdate(td_scene, int);

void forAllColliders(td_scene, void *, void *);

void executeUpdateBehaviors(lua_State *, td_scene, int);

void iterateAnimations(td_scene);

bool checkCollisionsWithoutCallbacks(td_boxCollider, td_linkedList);

bool checkCollisions(td_boxCollider, td_linkedList);

void resolveCollisions(td_scene);

void registerTimeout(td_scene, int, float);

void executeTimeouts(lua_State *, td_scene);

void executeEventBehaviors(lua_State *, td_scene, td_hashMap, SDL_Event);

td_linkedList getWorldColliders(td_scene);

td_linkedList getEntityColliders(td_scene);

td_entity getEntityByID(td_scene, char *);

td_camera getCameraByID(td_scene, char *);

void addAnimationToScene(td_scene, td_animation, char *);

void destroyScene(td_scene);

#endif