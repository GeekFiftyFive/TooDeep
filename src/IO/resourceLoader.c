#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
    void (*plaintextLoader)(char *path);
    void (*surfaceLoader)(char *path);
};

td_resourceLoader createResourceLoader(char *basePath) {
    return NULL;
}

char *loadPlaintextResource(td_resourceLoader rl, char *path) {
    return NULL;
}

SDL_Surface *loadSurfaceResource(td_resourceLoader rl, char *path) {
    return NULL;
}

void destroyResourceLoader(td_resourceLoader rl) {

}