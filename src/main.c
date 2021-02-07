#define SDL_MAIN_HANDLED
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Renderer/renderer.h"
#include "Events/eventLoop.h"
#include "Tests/testHelper.h"
#include "IO/fileIO.h"
#include "IO/logger.h"
#include "IO/gameLoader.h"
#include "IO/resourceLoader.h"
#include "JSON/jsonParser.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define USER_CONFIG_NAME "td-user-config.json"
#define MANIFEST "td-game.json"

bool shouldRunTests(int argc, char *args[]) {
    for(int i = 0; i < argc; i++) {
        if(strcmp(args[i], "-run-tests") == 0) return true;
    }

    return false;
}

int main(int argc, char *args[]) {
    if(shouldRunTests(argc, args)) return runTests();
    else if(argc < 2) {
        printf("Usage: tooDeep <game directory>\n");
        return 0;
    }

    td_resourceLoader loader = createResourceLoader(args[1]);
    char *configText = loadPlaintextResource(loader, USER_CONFIG_NAME);
    char *manifestText = loadPlaintextResource(loader, MANIFEST);

    if(!configText) {
        logError("Cannot load config file\n");
        destroyResourceLoader(loader);
        return 2;
    }

    td_json config = jsonParse(configText);
    td_json manifest = jsonParse(manifestText);

    int width = getJSONInt(config, "userConfig.resolution.w", NULL);
    int height = getJSONInt(config, "userConfig.resolution.h", NULL);
    char *title = getJSONString(manifest, "meta.title", NULL);
    bool fullscreen = getJSONBool(config, "userConfig.resolution.fullscreen", NULL);
    
    td_renderer renderer = initRenderer(title, width, height, fullscreen);

    if(!renderer) return 1;

    td_game game = loadGameFromDirectory(args[1], renderer);

    if(!game) {
        logError("Failed to start!\n");
        return 1;
    }

    startEventLoop(game);

    destroyResourceLoader(loader);

    destroyRenderer(renderer);

    destroyGame(game);

    quit();

    return 0;
}