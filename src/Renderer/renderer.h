#ifndef RENDERER__
#define RENDERER__

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "camera.h"
#include "../DataStructures/Tuple/tuple.h"

typedef enum {UI_SPACE, WORLD_SPACE} td_renderSpace;

typedef struct td_renderer *td_renderer;

typedef struct td_renderable *td_renderable;

td_renderer initRenderer(char *, int, int, bool);

void toggleCursor(bool);

SDL_Texture *surfaceToTexture(td_renderer, SDL_Surface *);

void appendToRenderQueue(td_renderer, td_renderable);

void clearRenderQueue(td_renderer);

td_renderable createRenderableFromTexture(td_renderer, SDL_Texture *);

td_renderable createRenderableFromSurface(td_renderer, SDL_Surface *);

void setCurrentCamera(td_renderer, td_camera);

void setRenderableFlip(td_renderable, SDL_RendererFlip);

void setRenderableTextureRegion(td_renderable, SDL_Rect);

td_tuple getRenderableSize(td_renderable);

void setRenderableSize(td_renderable, td_tuple);

td_tuple getRenderablePosition(td_renderable);

void setRenderablePosition(td_renderable, td_tuple);

void updateRenderablePosition(td_renderable, td_tuple);

void setRenderSpace(td_renderable, td_renderSpace);

void addDebugRect(td_renderer, td_tuple, td_tuple);

void renderFrame(td_renderer);

SDL_Rect getTextureRegion(SDL_Texture *, SDL_Rect, int);

void destroyRenderer(td_renderer);

void destroyRenderable(td_renderable);

#endif