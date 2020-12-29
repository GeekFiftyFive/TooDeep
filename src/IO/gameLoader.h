#ifndef GAME_LOADER
#define GAME_LOADER

#include "../JSON/jsonParser.h"
#include "../Renderer/renderer.h"
#include "resourceLoader.h"

typedef struct td_game *td_game;

typedef struct td_world *td_world;

td_game loadGameFromDirectory(char *, td_renderer);

void copySceneToRenderQueue(td_game game);

td_json getManifest(td_game);

td_json getScene(td_game, char*);

td_json getEntity(td_game, char*);

td_resourceLoader getResourceLoader(td_game);

td_renderer getRenderer(td_game);

char *newEntityID(td_game game);

void destroyGame(td_game);

#endif