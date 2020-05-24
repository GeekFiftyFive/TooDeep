#include <SDL2/SDL.h>
#include "Renderer/renderer.h"

int main(int argc, char *args[]) {
    int err = SDL_Init(SDL_INIT_VIDEO);

    if(err) {
        fprintf(stderr, "Error initialising SDL: %s\n", SDL_GetError());
        return 1;
    }

    td_renderer renderer = initRenderer();

    SDL_Event e;

    SDL_PollEvent(&e);

    SDL_Delay(2000);

    destroyRenderer(renderer);

    SDL_Quit();

    return 0;
}