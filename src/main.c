#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Renderer/renderer.h"
#include "Events/eventLoop.h"
#include "Tests/testHelper.h"
#include "IO/fileIO.h"
#include "IO/logger.h"
#include "JSON/jsonParser.h"
#include "IO/gameLoader.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

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

    td_game game = loadGameFromDirectory(args[1]);

    if(!game) {
        logError("Failed to start!\n");
        return 1;
    }

    td_json config = getConfig(game);
    td_json manifest = getManifest(game);

    int width = getJSONInt(config, "userConfig.resolution.w", NULL);
    int height = getJSONInt(config, "userConfig.resolution.h", NULL);

    char *title = getJSONString(manifest, "meta.title", NULL);

    td_renderer renderer = initRenderer(title, width, height);
    
    if(!renderer) return 1;

    startEventLoop(renderer, game);

    destroyRenderer(renderer);

    destroyGame(game);

    quit();

    return 0;
}