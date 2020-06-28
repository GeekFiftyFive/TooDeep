#ifndef RESOURCE_LOADER
#define RESOURCE_LOADER

typedef struct td_resourceLoader *td_resourceLoader;

td_resourceLoader createResourceLoader(char *);

char *loadPlaintextResource(td_resourceLoader, char*);

SDL_Surface *loadSurfaceResource(td_resourceLoader, char *);

void setPlaintextLoader(td_resourceLoader, char *(*loader)(char *));

void setSurfaceLoader(td_resourceLoader, SDL_Surface *(*loader)(char *));

void resetLoaders(td_resourceLoader);

void destroyResourceLoader(td_resourceLoader);

#endif