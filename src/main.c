#include <SDL2/SDL.h>
#include "Renderer/renderer.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char *args[]) {
    int err = SDL_Init(SDL_INIT_VIDEO);

    if(err) {
        fprintf(stderr, "Error initialising SDL: %s\n", SDL_GetError());
        return 1;
    }

    td_renderer renderer = initRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Event e;

    SDL_PollEvent(&e);

    SDL_Delay(2000);

    destroyRenderer(renderer);

    SDL_Quit();

    return 0;
}