#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "resourceLoader.h"
#include "logger.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "fileIO.h"

#define fontLoadError(path) logError("An error was encounted whilst loading the font %s\n", path); return NULL;
#define wavLoadError(path) logError("An error was encountered whilst loading the wav %s\n", path); return NULL;
#define surfaceLoadError(path) logError("An error was encountered whilst loading the surface %s\n", path); return NULL;

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
    char *(*plaintextLoader)(char *path);
    SDL_Surface *(*surfaceLoader)(char *path);
    Mix_Chunk *(*wavLoader)(char *path);
    SDL_RWops *(*genericSDLLoader)(char *path);
};

char *plaintextLoader(char *path) {
    return readFile(path);
}

SDL_Surface *surfaceLoader(char *path) {
    return IMG_Load(path);
}

Mix_Chunk *wavLoader(char *path) {
    return Mix_LoadWAV(path);
}

SDL_RWops *genericSDLLoader(char *path) {
    // Opens a binary file for reading
    return SDL_RWFromFile(path, "rb");
}

td_resourceLoader createResourceLoader(char *basePath) {
    td_resourceLoader resourceLoader = malloc(sizeof(struct td_resourceLoader));

    resourceLoader -> resources = createHashMap(30);
    resourceLoader -> basePath = basePath;
    resourceLoader -> plaintextLoader = plaintextLoader;
    resourceLoader -> surfaceLoader = surfaceLoader;
    resourceLoader -> wavLoader = wavLoader;
    resourceLoader -> genericSDLLoader = genericSDLLoader;

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
    free(fullPath);

    if(!surface) {
        surfaceLoadError(path);
    }

    insertIntoHashMap(rl -> resources, path, surface, SDL_FreeSurface);
    return surface;
}

Mix_Chunk *loadWavResource(td_resourceLoader rl, char *path) {
    Mix_Chunk *chunk = (Mix_Chunk*) getFromHashMap(rl -> resources, path);

    if(chunk) {
        return chunk;
    }

    char *fullPath = concatPath(rl -> basePath, path);

    chunk = rl -> wavLoader(fullPath);
    free(fullPath);

    if(!chunk) {
        wavLoadError(path);
    }

    insertIntoHashMap(rl -> resources, path, chunk, Mix_FreeChunk);
    return chunk;
}

TTF_Font *loadFontResource(td_resourceLoader rl, char *path, int pnt) {
    SDL_RWops *rawFont = (SDL_RWops*) getFromHashMap(rl -> resources, path);

    if(!rawFont) {
        char *fullPath = concatPath(rl -> basePath, path);
        rawFont = rl -> genericSDLLoader(fullPath);
        free(fullPath);
        if(!rawFont) {
            fontLoadError(path);
        }
        insertIntoHashMap(rl -> resources, path, rawFont, rawFont -> close);
    }

    TTF_Font *font = TTF_OpenFontRW(rawFont, 0, pnt);

    if(!font) {
        fontLoadError(path);
    }

    return font;
}

void setPlaintextLoader(td_resourceLoader rl, char *(*loader)(char *)) {
    rl -> plaintextLoader = loader;
}

void setSurfaceLoader(td_resourceLoader rl, SDL_Surface *(*loader)(char *)) {
    rl -> surfaceLoader = loader;
}

void setWavLoader(td_resourceLoader rl, Mix_Chunk *(*loader)(char *)) {
    rl -> wavLoader = loader;
}

void setGenericSDLLoader(td_resourceLoader rl, SDL_RWops *(*loader)(char *)) {
    rl -> genericSDLLoader = loader;
}

void resetLoaders(td_resourceLoader rl) {
    rl -> plaintextLoader = plaintextLoader;
    rl -> surfaceLoader = surfaceLoader;
    rl -> wavLoader = wavLoader;
    rl -> genericSDLLoader = genericSDLLoader;
}

void destroyResourceLoader(td_resourceLoader rl) {
    destroyHashMap(rl -> resources);
    free(rl);
}