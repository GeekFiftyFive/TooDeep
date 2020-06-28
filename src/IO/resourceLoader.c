#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
    char *(*plaintextLoader)(char *path);
    SDL_Surface *(*surfaceLoader)(char *path);
};

char *plaintextLoader(char *path) {
    return NULL;
}

SDL_Surface *surfaceLoader(char *path) {
    return NULL;
}

td_resourceLoader createResourceLoader(char *basePath) {
    td_resourceLoader resourceLoader = malloc(sizeof(struct td_resourceLoader));

    resourceLoader -> resources = createHashMap(30);
    resourceLoader -> basePath = basePath;
    resourceLoader -> plaintextLoader = plaintextLoader;
    resourceLoader -> surfaceLoader = surfaceLoader;

    return resourceLoader;
}

char *loadPlaintextResource(td_resourceLoader rl, char *path) {
    return NULL;
}

SDL_Surface *loadSurfaceResource(td_resourceLoader rl, char *path) {
    return NULL;
}

void setPlaintextLoader(td_resourceLoader rl, char *(*loader)(char *)) {
    rl -> plaintextLoader = loader;
}

void setSurfaceLoader(td_resourceLoader rl, SDL_Surface *(*loader)(char *)) {
    rl -> surfaceLoader = loader;
}

void resetLoaders(td_resourceLoader rl) {
    rl -> plaintextLoader = plaintextLoader;
    rl -> surfaceLoader = surfaceLoader;
}

void destroyResourceLoader(td_resourceLoader rl) {
    destroyHashMap(rl -> resources);
    free(rl);
}