#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Renderer/renderer.h"
#include "Events/eventLoop.h"
#include "Tests/testHelper.h"
#include "IO/fileIO.h"
#include "IO/logger.h"
#include "JSON/jsonParser.h"

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

    // TODO: Pull target file from the command arguments
    char *configFile = readFile("examples/spaceship/td-user-config.json");
    td_json config;

    int width = SCREEN_WIDTH, height = SCREEN_HEIGHT;

    if(configFile) {
        config = jsonParse(configFile);
        if(config) {
            width = getJSONInt(config, "userConfig.resolution.w", NULL);
            height = getJSONInt(config, "userConfig.resolution.h", NULL);
        }
        free(configFile);
    } else {
        fprintf(stderr, "Could not load config file!\n");
    }

    td_renderer renderer = initRenderer(width, height);

    td_renderable testRenderable = createRendereable("examples/spaceship/assets/rocket.png", renderer);
    
    if(!renderer) return 1;

    startEventLoop(renderer);

    destroyRenderer(renderer);

    freeJson(config);

    quit();

    return 0;
}