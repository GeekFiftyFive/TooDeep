#ifndef GAME_LOADER
#define GAME_LOADER

#include "../JSON/jsonParser.h"

typedef struct td_game *td_game;

td_game loadGameFromDirectory(char *);

td_json getConfig(td_game);

td_json getManifest(td_game);

void destroyGame(td_game);

#endif