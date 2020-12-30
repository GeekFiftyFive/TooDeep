#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../IO/gameLoader.h"
#include "../State/Entity/entity.h"

void startEventLoop(td_game game) {
    bool quit = false;
    SDL_Event e;

    copySceneToRenderQueue(game);

    while(!quit) {
        SDL_PollEvent(&e);
        quit = e.type == SDL_QUIT;
        renderFrame(getRenderer(game));
    }
}

void quit() {
    IMG_Quit();
    SDL_Quit();
}