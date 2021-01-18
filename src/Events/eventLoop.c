#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../IO/gameLoader.h"
#include "../IO/logger.h"
#include "../State/Entity/entity.h"

void startEventLoop(td_game game) {
    bool quit = false;
    SDL_Event e;

    int acc = 0;
    int count = 0;
    int prevTicks = SDL_GetTicks();

    copySceneToRenderQueue(game);

    while(!quit) {
        quit = e.type == SDL_QUIT;
        renderFrame(getRenderer(game));
        // FIXME: Doing this is overkill. Should just swap at renderables
        // that need to be swapped out when they need to be swapped out
        // e.g. when an animation begins playing
        clearRenderQueue(getRenderer(game));
        copySceneToRenderQueue(game);
        executeTick(game, SDL_GetTicks() - prevTicks);
        while(SDL_PollEvent(&e)) {
            executeEvent(game, e);
        }
        acc++;
        count += SDL_GetTicks() - prevTicks;
        prevTicks = SDL_GetTicks();
        if(acc == 100) {
            float avgTime = (count / 100);
            logDebug("FPS: %f\n", 1000 / avgTime);
            acc = 0;
            count = 0;
        }
    }
}

void quit() {
    IMG_Quit();
    SDL_Quit();
}