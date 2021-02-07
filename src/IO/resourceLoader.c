#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "resourceLoader.h"
#include "logger.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "fileIO.h"

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
    char *(*plaintextLoader)(char *path);
    SDL_Surface *(*surfaceLoader)(char *path);
};

char *plaintextLoader(char *path) {
    return readFile(path);
}

SDL_Surface *surfaceLoader(char *path) {
    return IMG_Load(path);
}

td_resourceLoader createResourceLoader(char *basePath) {
    td_resourceLoader resourceLoader = malloc(sizeof(struct td_resourceLoader));

    resourceLoader -> resources = createHashMap(30);
    resourceLoader -> basePath = basePath;
    resourceLoader -> plaintextLoader = plaintextLoader;
    resourceLoader -> surfaceLoader = surfaceLoader;

    return resourceLoader;
}

// TODO: Attempt to generalise this using macros
static char *loadPlaintextResourceOpt(td_resourceLoader rl, char *path, bool staticRes) {
    char *fullPath = staticRes ? path : concatPath(rl -> basePath, path);
    char *plaintext = (char*) getFromHashMap(rl -> resources, fullPath);
    
    if(plaintext) {
        return plaintext;
    }

    plaintext = rl -> plaintextLoader(fullPath);

    if(!plaintext) {
        free(fullPath);
        logError("Error loading file: %s\n", fullPath);
        return NULL;
    }

    insertIntoHashMap(rl -> resources, fullPath, plaintext, free);

    return plaintext;
}

char *loadStaticPlaintextResource(td_resourceLoader rl, char *path) {
    return loadPlaintextResourceOpt(rl, path, true);
}

char *loadPlaintextResource(td_resourceLoader rl, char *path) {
    return loadPlaintextResourceOpt(rl, path, false);
}

SDL_Surface *loadSurfaceResource(td_resourceLoader rl, char *path) {
    SDL_Surface *surface = (SDL_Surface*) getFromHashMap(rl -> resources, path);

    if(surface) {
        return surface;
    }

    char *fullPath = concatPath(rl -> basePath, path);

    surface = rl -> surfaceLoader(fullPath);
    insertIntoHashMap(rl -> resources, path, surface, SDL_FreeSurface);

    free(fullPath);
    return surface;
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