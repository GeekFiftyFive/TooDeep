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

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define USER_CONFIG_NAME "/td-user-config.json"
#define MANIFEST_NAME "/td-game.json"

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

    char *configDir = malloc(strlen(args[1]) + strlen(USER_CONFIG_NAME) + 1);
    char *manifestDir = malloc(strlen(args[1]) + strlen(MANIFEST_NAME) + 1);

    sprintf(configDir, "%s%s", args[1], USER_CONFIG_NAME);
    sprintf(manifestDir, "%s%s", args[1], MANIFEST_NAME);

    // TODO: Pull target file from the command arguments
    char *configFile = readFile(configDir);
    char *manifestFile = readFile(manifestDir);
    td_json config;
    td_json manifest;

    if(configFile) {
        config = jsonParse(configFile);
        if(!config) {
            logError("Could not parse config file!\n");
            return 1;
        }
        free(configFile);
    } else {
        logError("Could not load config file!\n");
        return 1;
    }

    int width = getJSONInt(config, "userConfig.resolution.w", NULL);
    int height = getJSONInt(config, "userConfig.resolution.h", NULL);

    if(manifestFile) {
        manifest = jsonParse(manifestFile);
        if(!manifest) {
            logError("Could not parse manifest!\n");
            return 2;
        }
        free(manifestFile);
    } else {
        logError("Could not load manifest!\n");
        return 2;
    }

    char *title = getJSONString(manifest, "meta.title", NULL);

    td_renderer renderer = initRenderer(title, width, height);

    td_renderable testRenderable = createRendereable("examples/spaceship/assets/rocket.png", renderer);
    
    if(!renderer) return 1;

    startEventLoop(renderer);

    destroyRenderer(renderer);

    freeJson(config);

    free(configDir);

    free(manifestDir);

    quit();

    return 0;
}