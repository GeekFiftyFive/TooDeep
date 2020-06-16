#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileIO.h"

char *basePath = "/";

char *readFile(const char *path) {
    char *fullPath = malloc(strlen(path) + strlen(basePath) + 1);
    sprintf(fullPath, "%s%s", basePath, path);

    char *buffer = 0;
    long length = 0;
    FILE *f = fopen(fullPath, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer) fread(buffer, 1, length, f);
        fclose(f);
    }

    buffer[length] = '\0';
    free(fullPath);

    return buffer;
}

void setBasePath(char *newBasePath) {
    basePath = newBasePath;
}