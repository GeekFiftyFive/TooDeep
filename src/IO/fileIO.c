#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "fileIO.h"

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

int iterateOverDir(char *path, void (*callback)(char *, void *), void *data) {
    DIR *dp;
    struct dirent *ep;
    int count = 0;

    dp = opendir(path);
    if(dp) {
        while(ep = readdir(dp)) {
            count++;
            callback(ep -> d_name, data);
        }
        closedir(dp);
    }

    return count;
}