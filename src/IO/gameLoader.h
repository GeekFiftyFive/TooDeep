#ifndef GAME_LOADER
#define GAME_LOADER

#include "../JSON/jsonParser.h"

typedef struct td_game *td_game;

typedef struct td_world *td_world;

td_game loadGameFromDirectory(char *);

td_json getConfig(td_game);

td_json getManifest(td_game);

td_json getScene(td_game, const char*);

void destroyGame(td_game);

#endif