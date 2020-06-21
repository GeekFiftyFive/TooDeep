#ifndef RESOURCE_LOADER
#define RESOURCE_LOADER

typedef struct td_resourceLoader *td_resourceLoader;

td_resourceLoader createResourceLoader(char *);

char *loadPlaintext(td_resourceLoader, char*);

SDL_Texture *loadTexture(td_resourceLoader, char *);

void destroyResourceLoader(td_resourceLoader);

#endif