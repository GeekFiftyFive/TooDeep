#include <stdio.h>
#include <SDL2/SDL.h>
#include "renderer.h"

struct td_renderer {
    SDL_Window *window;
};

/*
    Initialises the main window and returns the renderer struct.
    SDL must have bee initialised before this can be called.
*/
td_renderer initRenderer(int width, int height) {
    // The main game window
    SDL_Window *window = NULL;

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

    // Get the window's surface
    screen = SDL_GetWindowSurface(window);

    // Clear the surface
    SDL_FillRect(screen, NULL, SDL_MapRGB( screen -> format, 0xFF, 0xFF, 0xFF ));

    SDL_UpdateWindowSurface(window);

    td_renderer renderer = malloc(sizeof(struct td_renderer));

    renderer -> window = window;

    return renderer;
}

/*
    Frees a td_renderer pointer
*/
void destroyRenderer(td_renderer renderer) {
    SDL_DestroyWindow(renderer -> window);
    free(renderer);
}