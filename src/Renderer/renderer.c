#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>
#include "renderer.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"

#define BASE_WIDTH 1920

struct td_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    td_linkedList renderQueue;
    float scaleFactor;
};

struct td_renderable {
    SDL_Texture *texture;
    SDL_Rect rect;
    td_renderSpace space;
    int num;
};

/*
    Initialises the main window and returns the renderer struct.
    SDL must have bee initialised before this can be called.
*/
td_renderer initRenderer(char *title, int width, int height) {
    int err = SDL_Init(SDL_INIT_VIDEO);

    if(err) {
        logError("Error initialising SDL: %s\n", SDL_GetError());
        return NULL;
    }

    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
        logError("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return NULL;
    }

    // The main game window
    SDL_Window *window = NULL;

    // The main renderer
    SDL_Renderer *sdlRenderer = NULL;

    //Create the window
    window = SDL_CreateWindow(  title, 
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                width,
                                height,
                                SDL_WINDOW_SHOWN );

    if(!window) {
        logError("Unable to initialise the window: %s\n", SDL_GetError());
        return NULL;
    }

    // Create the accelerated renderer
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(!sdlRenderer) {
        logError("Unable to initialise the renderer: %s\n", SDL_GetError());
    }

    td_renderer renderer = malloc(sizeof(struct td_renderer));

    renderer -> window = window;
    renderer -> renderer = sdlRenderer;
    renderer -> renderQueue = createLinkedList();
    renderer -> scaleFactor = (float) width / BASE_WIDTH;

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
    if( loadedSurface == NULL ) {
        logError("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
        failure = true;
    } else {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer -> renderer, loadedSurface );
        if( newTexture == NULL ) {
            logError("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
            failure = true;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    if(failure) return NULL;
    else return newTexture;
}

/*
Function to be called when the render queue is destroyed in order to
free up renderables
*/
void renderableFreeFunc(void* renderableData) {
    td_renderable renderable = (td_renderable) renderableData;
    SDL_DestroyTexture(renderable -> texture);
    free(renderable);
}

td_renderable createRendereable(const char *path, td_renderer renderer) {
    td_renderable renderable = malloc(sizeof(struct td_renderable));

    renderable -> texture = loadTexture(path, renderer);

    int queueLength = listLength(renderer -> renderQueue);

    // Create a unique key
    char *key = malloc((int) ceil(log10(queueLength + 1)) + 1);

    sprintf(key, "%d", queueLength + 1);

    appendWithFree(renderer -> renderQueue, renderable, key, renderableFreeFunc);

    renderable -> num = queueLength + 1;

    SDL_QueryTexture(renderable -> texture, NULL, NULL, &renderable -> rect.w, &renderable -> rect.h);

    renderable -> rect.x = 0;
    renderable -> rect.y = 0;
    renderable -> space = WORLD_SPACE;

    return renderable;
}

void setRenderSpace(td_renderable renderable, td_renderSpace space) {
    renderable -> space = space;
}

SDL_Rect scaleRect(SDL_Rect rect, td_renderer renderer) {
    float scaleFactor = renderer -> scaleFactor;

    SDL_Rect scaled = {
        rect.x * scaleFactor,
        rect.y * scaleFactor,
        rect.w * scaleFactor,
        rect.h * scaleFactor
    };

    return scaled;
}

void drawRenderable(void *renderableData, void *rendererData, char *key) {
    td_renderable renderable = (td_renderable) renderableData;
    td_renderer renderer = (td_renderer) rendererData;

    SDL_Rect drawArea = scaleRect(renderable -> rect, renderer);

    SDL_RenderCopyEx(renderer -> renderer, renderable -> texture, NULL, &drawArea, 0, NULL, SDL_FLIP_NONE);
}

void renderFrame(td_renderer renderer) {
    SDL_SetRenderDrawColor(renderer -> renderer, 66, 134, 244, 0xFF );

    listForEach(renderer -> renderQueue, drawRenderable, renderer);

    SDL_RenderPresent(renderer -> renderer);
    SDL_RenderClear(renderer -> renderer);
}

/*
    Frees a td_renderer pointer
*/
void destroyRenderer(td_renderer renderer) {
    SDL_DestroyWindow(renderer -> window);
    SDL_DestroyRenderer(renderer -> renderer);
    destroyLinkedList(renderer -> renderQueue);
    free(renderer);
}