#ifndef RESOURCE_LOADER
#define RESOURCE_LOADER

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct td_resourceLoader *td_resourceLoader;

td_resourceLoader createResourceLoader(char *);

char *loadStaticPlaintextResource(td_resourceLoader, char *);

char *loadPlaintextResource(td_resourceLoader, char *);

SDL_Surface *loadSurfaceResource(td_resourceLoader, char *);

Mix_Chunk *loadWavResource(td_resourceLoader, char *);

void setPlaintextLoader(td_resourceLoader, char *(*loader)(char *));

void setSurfaceLoader(td_resourceLoader, SDL_Surface *(*loader)(char *));

void setWavLoader(td_resourceLoader, Mix_Chunk *(*loader)(char *));

void resetLoaders(td_resourceLoader);

void destroyResourceLoader(td_resourceLoader);

#endif