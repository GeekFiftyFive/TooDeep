#ifndef RESOURCE_LOADER
#define RESOURCE_LOADER

typedef struct td_resourceLoader *td_resourceLoader;

td_resourceLoader createResourceLoader(char *);

char *loadPlaintextResource(td_resourceLoader, char*);

SDL_Surface *loadSurfaceResource(td_resourceLoader, char *);

void destroyResourceLoader(td_resourceLoader);

#endif