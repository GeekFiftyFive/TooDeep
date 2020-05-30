#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Renderer/renderer.h"
#include "Events/eventLoop.h"
#include "Tests/testHelper.h"
#include "IO/fileIO.h"
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
    char *configFile = readFile("examples/spaceship/td.json");

    if(configFile) {
        jsonParse(configFile);
        free(configFile);
    }

    td_renderer renderer = initRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    td_renderable testRenderable = createRendereable("examples/spaceship/assets/rocket.png", renderer);
    
    if(!renderer) return 1;

    startEventLoop(renderer);

    destroyRenderer(renderer);

    quit();

    return 0;
}