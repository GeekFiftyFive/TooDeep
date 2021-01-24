#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../IO/gameLoader.h"
#include "../IO/logger.h"
#include "../State/Entity/entity.h"

#ifndef RENDER_DEBUG_DATA
#define RENDER_DEBUG_DATA false
#endif
#define debug(x) if(RENDER_DEBUG_DATA) x

void startEventLoop(td_game game) {
    bool quit = false;
    SDL_Event e;

    int acc = 0;
    int prevTicks = SDL_GetTicks();
    int startCount = prevTicks;

    copySceneToRenderQueue(game);
    debug(createDebugRenderables(game));

    while(!quit) {
        quit = e.type == SDL_QUIT;
        if(SDL_GetTicks() == prevTicks) {
            continue;
        }
        renderFrame(getRenderer(game));
        // FIXME: Doing this is overkill. Should just swap at renderables
        // that need to be swapped out when they need to be swapped out
        // e.g. when an animation begins playing
        clearRenderQueue(getRenderer(game));
        copySceneToRenderQueue(game);
        debug(createDebugRenderables(game));
        int delta = SDL_GetTicks() - prevTicks;
        prevTicks = SDL_GetTicks();
        executeTick(game, delta);
        while(SDL_PollEvent(&e)) {
            executeEvent(game, e);
        }
        acc++;
        if(SDL_GetTicks() - startCount > 1000) {
            logDebug("FPS: %d\n", acc);
            acc = 0;
            startCount = SDL_GetTicks();
        }
    }
}

void quit() {
    IMG_Quit();
    SDL_Quit();
}