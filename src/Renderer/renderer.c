#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "renderer.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../IO/logger.h"
#include "../Utils/stringUtils.h"

#define BASE_WIDTH 1920

struct td_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    td_linkedList renderQueue;
    td_camera camera;
    float scaleFactor;
};

struct td_renderable {
    SDL_Texture *texture;
    td_tuple pos;
    td_tuple size;
    td_renderSpace space;
    SDL_Rect *textureRegion;
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
    renderer -> camera = NULL;

    return renderer;
}

/*
Private helper function to create a texture from a surface
*/
SDL_Texture *surfaceToTexture(td_renderer renderer, SDL_Surface *surface) {
    if(!surface) return NULL;

    //The final texture
    SDL_Texture *newTexture = NULL;
    //Checks if something went wrong
    bool failure = false;

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( renderer -> renderer, surface );
    if( newTexture == NULL ) {
        failure = true;
    }

    if(failure) return NULL;
    else return newTexture;
}

void appendToRenderQueue(td_renderer renderer, td_renderable renderable) {
    int queueLength = listLength(renderer -> renderQueue);

    // Create a unique key
    char *key = stringifyInt(queueLength + 1);

    append(renderer -> renderQueue, renderable, key);
}

td_renderable createRenderableFromTexture(td_renderer renderer, SDL_Texture *texture) {
    td_renderable renderable = malloc(sizeof(struct td_renderable));

    renderable -> texture = texture;

    int w, h;

    SDL_QueryTexture(renderable -> texture, NULL, NULL, &w, &h);

    renderable -> pos.x  = 0.0;
    renderable -> pos.y  = 0.0;
    renderable -> size.x = (float) w;
    renderable -> size.y = (float) h;
    renderable -> space  = WORLD_SPACE;
    renderable -> textureRegion = NULL;

    return renderable;
}

td_renderable createRenderableFromSurface(td_renderer renderer, SDL_Surface *surface) {
    SDL_Texture *texture = surfaceToTexture(renderer, surface);

    return createRenderableFromTexture(renderer, texture);
}

void setCurrentCamera(td_renderer renderer, td_camera camera) {
    renderer -> camera = camera;
}

void setRenderableTextureRegion(td_renderable renderable, SDL_Rect region) {
    if(!renderable -> textureRegion) {
        renderable -> textureRegion = malloc(sizeof(SDL_Rect));
    }

    *(renderable -> textureRegion) = region;
}

void setRenderableSize(td_renderable renderable, td_tuple size) {
    renderable -> size = size;
}

void setRenderablePosition(td_renderable renderable, td_tuple pos) {
    renderable -> pos = pos;
}

void updateRenderablePosition(td_renderable renderable, td_tuple delta) {
    renderable -> pos = addTuple(renderable -> pos, delta);
}

void setRenderSpace(td_renderable renderable, td_renderSpace space) {
    renderable -> space = space;
}

SDL_Rect scaleRect(td_tuple pos, td_tuple size, td_renderer renderer) {
    float scaleFactor = renderer -> scaleFactor;
    td_tuple cameraPosition = getCameraPosition(renderer -> camera);
    float zoom = getCameraZoom(renderer -> camera);

    scaleFactor *= zoom;

    SDL_Rect scaled = {
        (pos.x - cameraPosition.x) * scaleFactor,
        (pos.y - cameraPosition.y) * -scaleFactor,
        size.x * scaleFactor,
        size.y * scaleFactor
    };

    return scaled;
}

void drawRenderable(void *renderableData, void *rendererData, char *key) {
    td_renderable renderable = (td_renderable) renderableData;
    td_renderer renderer = (td_renderer) rendererData;

    SDL_Rect drawArea = scaleRect(renderable -> pos, renderable -> size, renderer);

    SDL_RenderCopyEx(
        renderer -> renderer,
        renderable -> texture,
        renderable -> textureRegion,
        &drawArea, 
        0,
        NULL,
        SDL_FLIP_NONE
    );
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

void destroyRenderable(td_renderable renderable) {
    SDL_DestroyTexture(renderable -> texture);
    if(renderable -> textureRegion) {
        free(renderable -> textureRegion);
    }
    free(renderable);
}