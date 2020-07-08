#ifndef FILE_IO
#define FILE_IO

#include <stdbool.h>

char *readFile(const char *);

int iterateOverDir(char *, bool, void (*callback)(char *, void *), void *);

char *concatPath(char *, char*);

#endif