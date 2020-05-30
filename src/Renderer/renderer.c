#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "renderer.h"

struct td_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

struct td_renderable {
    SDL_Texture *texture;
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
/*
Private helper function to load textures from a path
*/
SDL_Texture *loadTexture(const char *path, td_renderer renderer){
    //The final texture
    SDL_Texture *newTexture = NULL;
    //Checks if something went wrong
    bool failure = false;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL )
    {
        fprintf(stderr, "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
        failure = true;
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer -> renderer, loadedSurface );
        if( newTexture == NULL )
        {
            fprintf(stderr, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
            failure = true;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    if(failure) return NULL;
    else return newTexture;
}

td_renderable createRendereable(const char *path, td_renderer renderer) {
    td_renderable renderable = malloc(sizeof(struct td_renderable));

    renderable -> texture = loadTexture(path, renderer);

    //TODO: Register the renderable with the renderer

    return renderable;
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