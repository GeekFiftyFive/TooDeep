#include <SDL2/SDL.h>
#include "Renderer/renderer.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char *args[]) {
    td_renderer renderer = initRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!renderer) {
        return 1;
    }

    SDL_Event e;

    SDL_PollEvent(&e);

    SDL_Delay(2000);

    destroyRenderer(renderer);

    SDL_Quit();

    return 0;
}