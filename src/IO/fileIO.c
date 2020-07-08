#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "fileIO.h"

// Workaround for bogus Intellisense error
#ifdef __INTELLISENSE__
#define DT_DIR 4
#endif

char *readFile(const char *path) {
    char *buffer = 0;
    long length = 0;
    FILE *f = fopen(path, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer) fread(buffer, 1, length, f);
        fclose(f);
    }

    buffer[length] = '\0';

    return buffer;
}

static bool isDirectory(struct dirent *ep) {
    return ep -> d_type == DT_DIR;
}

static bool isRelative(struct dirent *ep) {
    return !strcmp(".", ep -> d_name) || !strcmp("..", ep -> d_name);
}

int iterateOverDir(char *path, bool recurse, void (*callback)(char *, void *), void *data) {
    DIR *dp;
    struct dirent *ep;
    int count = 0;

    dp = opendir(path);
    if(dp) {
        while((ep = readdir(dp))) {
            if((!isDirectory(ep) || recurse) && !isRelative(ep)) {
                count++;
                callback(ep -> d_name, data);
            }
        }
        closedir(dp);
    }

    return count;
}

char *concatPath(char *basePath, char *path) {
    char *fullPath = malloc(strlen(path) + strlen(basePath) + 1);
    sprintf(fullPath, "%s/%s", basePath, path);
    return fullPath;
}