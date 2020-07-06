#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "eventLoop.h"

void startEventLoop(td_renderer renderer, td_game game) {
    bool quit = false;
    SDL_Event e;

    while(!quit) {
        SDL_PollEvent(&e);
        quit = e.type == SDL_QUIT;
        renderFrame(renderer);
    }
}

void quit() {
    IMG_Quit();
    SDL_Quit();
}