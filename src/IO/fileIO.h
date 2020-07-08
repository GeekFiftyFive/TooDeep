#ifndef FILE_IO
#define FILE_IO

char *readFile(const char *path);

int iterateOverDir(char *path, void (*callback)(char *, void *), void *data);

char *concatPath(char *basePath, char* path);

#endif