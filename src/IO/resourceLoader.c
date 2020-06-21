#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
};

td_resourceLoader createResourceLoader(char *basePath) {
    return NULL;
}

char *loadPlaintext(td_resourceLoader rl, char *path) {
    return NULL;
}

SDL_Texture *loadTexture(td_resourceLoader rl, char *path) {
    
}

void destroyResourceLoader(td_resourceLoader rl) {

}