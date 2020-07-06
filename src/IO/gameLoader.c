#include <stdlib.h>
#include "gameLoader.h"
#include "resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../JSON/jsonParser.h"

struct td_game {
    td_resourceLoader loader;
    td_hashMap scenes;
    td_json config;
    char *title;
};

td_game loadGameFromDirectory(char *path) {
    td_game game = malloc(sizeof(struct td_game));

    game -> loader = createResourceLoader(path);
    game -> scenes = createHashMap(10);

    return game;
}

void destroyGame(td_game game) {
    destroyResourceLoader(game -> loader);
    destroyHashMap(game -> scenes);
    freeJson(game -> config);
    free(game -> title);
    free(game);
}