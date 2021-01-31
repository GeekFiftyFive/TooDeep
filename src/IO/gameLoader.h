#ifndef GAME_LOADER
#define GAME_LOADER

#include <lua.h>
#include "resourceLoader.h"
#include "../JSON/jsonParser.h"
#include "../Renderer/renderer.h"
#include <SDL2/SDL.h>

typedef struct td_game *td_game;

td_game loadGameFromDirectory(char *, td_renderer);

void copySceneToRenderQueue(td_game);

void createDebugRenderables(td_game);

void executeTick(td_game, int);

void executeEvent(td_game, SDL_Event);

lua_State *getState(td_game);

td_json getManifest(td_game);

td_json getScene(td_game, char*);

td_json getEntity(td_game, char*);

td_json getTileset(td_game, char*);

td_json getAnimation(td_game, char *);

td_json getStateMachine(td_game, char *);

td_resourceLoader getResourceLoader(td_game);

td_renderer getRenderer(td_game);

char *newEntityID(td_game);

void destroyGame(td_game);

#endif