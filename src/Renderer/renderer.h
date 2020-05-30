#ifndef RENDERER__
#define RENDERER__

typedef enum {UI_SPACE, WORLD_SPACE} td_renderSpace;

typedef struct td_renderer *td_renderer;

typedef struct td_renderable *td_renderable;

td_renderer initRenderer(int,int);

td_renderable createRendereable(const char*, td_renderer);

void setRenderSpace(td_renderable, td_renderSpace);

void renderFrame(td_renderer);

void destroyRenderer(td_renderer);

#endif