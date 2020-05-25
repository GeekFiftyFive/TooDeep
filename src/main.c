#include "Renderer/renderer.h"
#include "Events/eventLoop.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char *args[]) {
    td_renderer renderer = initRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!renderer) {
        return 1;
    }

    startEventLoop(renderer);

    destroyRenderer(renderer);

    quit();

    return 0;
}