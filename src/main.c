#include <stdbool.h>
#include <string.h>
#include "Renderer/renderer.h"
#include "Events/eventLoop.h"
#include "Tests/testHelper.h"

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

    td_renderer renderer = initRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    td_renderable testRenderable = createRendereable("examples/spaceship/assets/rocket.png", renderer);
    
    if(!renderer) return 1;

    startEventLoop(renderer);

    destroyRenderer(renderer);

    quit();

    return 0;
}