#include <stdio.h>
#include <stdlib.h>
#include "fileIO.h"

char *readFile(const char *path) {
    char *buffer = 0;
    long length;
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