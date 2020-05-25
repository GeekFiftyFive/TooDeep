#include <stdbool.h>
#include <SDL2/SDL.h>
#include "eventLoop.h"

void startEventLoop(td_renderer renderer) {
    bool quit = false;
    SDL_Event e;

    while(!quit) {
        SDL_PollEvent(&e);
        quit = e.type == SDL_QUIT;
        renderFrame(renderer);
    }
}

void quit() {
    SDL_Quit();
}