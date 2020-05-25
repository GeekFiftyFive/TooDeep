#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "renderer.h"

struct td_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

/*
    Initialises the main window and returns the renderer struct.
    SDL must have bee initialised before this can be called.
*/
td_renderer initRenderer(int width, int height) {
    int err = SDL_Init(SDL_INIT_VIDEO);

    if(err) {
        fprintf(stderr, "Error initialising SDL: %s\n", SDL_GetError());
        return NULL;
    }

    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return NULL;
    }

    // The main game window
    SDL_Window *window = NULL;

    // The main renderer
    SDL_Renderer *sdlRenderer = NULL;

    // The Window Surface
    SDL_Surface *screen = NULL;

    //Create the window
    window = SDL_CreateWindow( "TooDeep", 
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                width,
                                height,
                                SDL_WINDOW_SHOWN );

    if(!window) {
        fprintf(stderr, "Unable to initialise the window: %s\n", SDL_GetError());
        return NULL;
    }

    // Create the accelerated renderer
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!sdlRenderer) {
        fprintf(stderr, "Unable to initialise the renderer: %s\n", SDL_GetError());
    }

    td_renderer renderer = malloc(sizeof(struct td_renderer));

    renderer -> window = window;

    renderer -> renderer = sdlRenderer;

    return renderer;
}

void renderFrame(td_renderer renderer) {
    SDL_SetRenderDrawColor(renderer -> renderer, 66, 134, 244, 0xFF );

    SDL_RenderPresent(renderer -> renderer);

    SDL_RenderClear(renderer -> renderer);
}

/*
    Frees a td_renderer pointer
*/
void destroyRenderer(td_renderer renderer) {
    SDL_DestroyWindow(renderer -> window);
    SDL_DestroyRenderer(renderer -> renderer);
    free(renderer);
}