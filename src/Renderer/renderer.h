#ifndef RENDERER__
#define RENDERER__

typedef struct td_renderer *td_renderer;

td_renderer initRenderer(int,int);

void destroyRenderer(td_renderer);

#endif