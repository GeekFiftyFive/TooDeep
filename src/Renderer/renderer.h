#ifndef RENDERER__
#define RENDERER__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef enum {UI_SPACE, WORLD_SPACE} td_renderSpace;

typedef struct td_renderer *td_renderer;

typedef struct td_renderable *td_renderable;

td_renderer initRenderer(char *, int,int);

td_renderable createRenderableFromSurface(td_renderer, SDL_Surface *);

void setRenderSpace(td_renderable, td_renderSpace);

void renderFrame(td_renderer);

void destroyRenderer(td_renderer);

#endif