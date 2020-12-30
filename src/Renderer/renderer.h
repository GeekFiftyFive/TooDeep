#ifndef RENDERER__
#define RENDERER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../DataStructures/Tuple/tuple.h"

typedef enum {UI_SPACE, WORLD_SPACE} td_renderSpace;

typedef struct td_renderer *td_renderer;

typedef struct td_renderable *td_renderable;

td_renderer initRenderer(char *, int,int);

void appendToRenderQueue(td_renderer, td_renderable);

td_renderable createRenderableFromSurface(td_renderer, SDL_Surface *);

void setRenderablePosition(td_renderable, td_tuple);

void setRenderSpace(td_renderable, td_renderSpace);

void renderFrame(td_renderer);

void destroyRenderer(td_renderer);

void destroyRenderable(td_renderable);

#endif