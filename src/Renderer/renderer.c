#include <stdio.h>
#include <math.h>
#include "renderer.h"
#include "../DataStructures/LinkedList/linkedList.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "../IO/logger.h"
#include "../Utils/stringUtils.h"

#define BASE_WIDTH 1920

struct td_renderer {
    SDL_Window *window;
    SDL_Renderer *renderer;
    td_linkedList renderQueue;
    td_linkedList debugLayer;
    td_camera camera;
    float scaleFactor;
    SDL_Rect frameDimensions;
    td_hashMap textures;
};

struct td_renderable {
    SDL_Texture *texture;
    td_tuple pos;
    td_tuple size;
    td_renderSpace space;
    SDL_Rect *textureRegion;
    SDL_RendererFlip flip;
};

struct td_debugRenderable {
    SDL_Rect rect;
};

/*
    Initialises the main window and returns the renderer struct.
    SDL must have bee initialised before this can be called.
*/
td_renderer initRenderer(char *title, int width, int height, bool fullscreen) {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
        logError( "Could not set texture scaling method SDL_Error: %s\n", SDL_GetError());
    }

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
    SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    if(!window) {
        logError("Unable to initialise the window: %s\n", SDL_GetError());
        return NULL;
    }

    // Create the accelerated renderer
#ifdef __APPLE__
    // Workaround for weird issue with full screen accelerated renderer on macOS
    sdlRenderer = SDL_CreateRenderer(window, -1, fullscreen ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED);
#else
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#endif

    if(!sdlRenderer) {
        logError("Unable to initialise the renderer: %s\n", SDL_GetError());
    }

    td_renderer renderer = malloc(sizeof(struct td_renderer));

    renderer -> window = window;
    renderer -> renderer = sdlRenderer;
    renderer -> renderQueue = createLinkedList();
    renderer -> scaleFactor = (float) width / BASE_WIDTH;
    renderer -> camera = NULL;
    renderer -> frameDimensions = (SDL_Rect) { 0, 0, width, height };
    renderer -> debugLayer = createLinkedList();
    renderer -> textures = createHashMap(10);

    return renderer;
}

void toggleCursor(bool toggle) {
    SDL_ShowCursor(toggle);
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

    // FIXME: Just hash the pointer itself
    char *name = stringifyInt((int) surface);

    newTexture = getFromHashMap(renderer -> textures, name);
    if(newTexture) {
        return newTexture;
    }

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( renderer -> renderer, surface );
    if( newTexture == NULL ) {
        failure = true;
    } else {
        insertIntoHashMap(renderer -> textures, name, newTexture, SDL_DestroyTexture);
    }

    free(name);

    if(failure) return NULL;
    else return newTexture;
}

void appendToRenderQueue(td_renderer renderer, td_renderable renderable) {
    int queueLength = listLength(renderer -> renderQueue);

    // Create a unique key
    char *key = stringifyInt(queueLength + 1);

    append(renderer -> renderQueue, renderable, key);
    free(key);
}

void clearRenderQueue(td_renderer renderer) {
    destroyLinkedList(renderer -> renderQueue);
    destroyLinkedList(renderer -> debugLayer);
    renderer -> renderQueue = createLinkedList();
    renderer -> debugLayer = createLinkedList();
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
    renderable -> flip = SDL_FLIP_NONE;

    return renderable;
}

td_renderable createRenderableFromSurface(td_renderer renderer, SDL_Surface *surface) {
    SDL_Texture *texture = surfaceToTexture(renderer, surface);

    return createRenderableFromTexture(renderer, texture);
}

void setRenderSpace(td_renderable renderable, td_renderSpace space) {
    renderable -> space = space;
}

void setCurrentCamera(td_renderer renderer, td_camera camera) {
    renderer -> camera = camera;
}

void setRenderableFlip(td_renderable renderable, SDL_RendererFlip flip) {
    renderable -> flip = flip;
}

void setRenderableTextureRegion(td_renderable renderable, SDL_Rect region) {
    if(!renderable -> textureRegion) {
        renderable -> textureRegion = malloc(sizeof(SDL_Rect));
    }

    *(renderable -> textureRegion) = region;
}

td_tuple getRenderableSize(td_renderable renderable) {
    return renderable -> size;
}

void setRenderableSize(td_renderable renderable, td_tuple size) {
    renderable -> size = size;
}

td_tuple getRenderablePosition(td_renderable renderable) {
    return renderable -> pos;
}

void setRenderablePosition(td_renderable renderable, td_tuple pos) {
    renderable -> pos = pos;
}

void updateRenderablePosition(td_renderable renderable, td_tuple delta) {
    renderable -> pos = addTuple(renderable -> pos, delta);
}

SDL_Rect scaleRect(td_tuple pos, td_tuple size, td_renderer renderer) {
    float scaleFactor = renderer -> scaleFactor;
    td_tuple cameraPosition = getCameraPosition(renderer -> camera);
    float zoom = getCameraZoom(renderer -> camera);

    scaleFactor *= zoom;

    int xOffset = renderer -> frameDimensions.w / 2;
    int yOffset = renderer -> frameDimensions.h / 2;

    SDL_Rect scaled = {
        ((pos.x - cameraPosition.x) * scaleFactor) + xOffset,
        ((pos.y - cameraPosition.y) * -scaleFactor) + yOffset,
        size.x * scaleFactor,
        size.y * scaleFactor
    };

    return scaled;
}

SDL_Rect scaleScreenSpaceRect(td_tuple pos, td_tuple size, td_renderer renderer) {
    float scaleFactor = renderer -> scaleFactor;

    SDL_Rect scaled = {
        pos.x  * scaleFactor,
        pos.y  * scaleFactor,
        size.x * scaleFactor,
        size.y * scaleFactor
    };

    return scaled;
}

void addDebugRect(td_renderer renderer, td_tuple pos, td_tuple size) {
    struct td_debugRenderable *renderable = malloc(sizeof(struct td_debugRenderable));
    SDL_Rect rect = scaleRect(pos, size, renderer);
    renderable -> rect = rect;
    appendWithFree(renderer -> debugLayer, renderable, NULL, free);
}

static void drawRenderable(void *renderableData, void *rendererData, char *key) {
    td_renderable renderable = (td_renderable) renderableData;
    td_renderer renderer = (td_renderer) rendererData;
    SDL_Rect drawArea;

    if(renderable -> space == WORLD_SPACE) {
        drawArea = scaleRect(renderable -> pos, renderable -> size, renderer);
    } else if(renderable -> space == SCREEN_SPACE) {
        drawArea = scaleScreenSpaceRect(renderable -> pos, renderable -> size, renderer);
    }

    SDL_RenderCopyEx(
        renderer -> renderer,
        renderable -> texture,
        renderable -> textureRegion,
        &drawArea,
        0,
        NULL,
        renderable -> flip
    );
}

static void resetDrawColor(td_renderer renderer) {
    SDL_SetRenderDrawColor(renderer -> renderer, 66, 134, 244, 0xFF );
}

static void drawDebugRenderable(void *renderableData, void *rendererData, char *key) {
    struct td_debugRenderable *renderable = (struct td_debugRenderable *) renderableData;
    td_renderer renderer = (td_renderer) rendererData;

    SDL_SetRenderDrawColor(renderer -> renderer, 0xFF, 0x0, 0xFF, 0xFF );
    SDL_RenderDrawRect(renderer -> renderer, &renderable -> rect);
    resetDrawColor(renderer);
}

void renderFrame(td_renderer renderer) {
    resetDrawColor(renderer);
    SDL_RenderClear(renderer -> renderer);
    listForEach(renderer -> renderQueue, drawRenderable, renderer);
    listForEach(renderer -> debugLayer, drawDebugRenderable, renderer);

    SDL_RenderPresent(renderer -> renderer);
}

SDL_Rect getTextureRegion(SDL_Texture *texture, SDL_Rect dimensions, int index) {
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect region = dimensions;
    int tilesPerRow = w / dimensions.w;
    int row = ceil((float) index / tilesPerRow);
    region.x = ((index - 1) % tilesPerRow) * dimensions.w;
    region.y = dimensions.h * (row - 1);
    return region;
}

/*
    Frees a td_renderer pointer
*/
void destroyRenderer(td_renderer renderer) {
    SDL_DestroyWindow(renderer -> window);
    SDL_DestroyRenderer(renderer -> renderer);
    destroyLinkedList(renderer -> renderQueue);
    destroyLinkedList(renderer -> debugLayer);
    destroyHashMap(renderer -> textures);
    free(renderer);
}

void destroyRenderable(td_renderable renderable) {
    // Texture should be destroyed by the renderer!
    if(renderable -> textureRegion) {
        free(renderable -> textureRegion);
    }
    free(renderable);
}