#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "stringUtils.h"

struct td_string {
    char *cString;
    int capacity;
    int length;
};

td_string createString() {
    td_string string = malloc(sizeof(struct td_string));

    string -> capacity = 4;
    string -> cString = malloc(string -> capacity);
    string -> length = 0;

    return string;
}

char *toCString(td_string string) {
    char *out = malloc(string -> length + 1);
    strcpy(out, string -> cString);
    return out;
}

void appendString(td_string string, char *app) {
    int len = strlen(app);

    if(string -> length + len > string -> capacity / 2) {
        string -> capacity *= (2 * (1 + len / string -> capacity));
        string -> cString = realloc(string -> cString, string -> capacity);
    }

    memcpy(string -> cString + string -> length, app, len);
    string -> length += len;
    string -> cString[string -> length] = '\0';
}

void destroyString(td_string string) {
    free(string -> cString);
    free(string);
}

char *stringifyInt(unsigned int num) {
    char *str = malloc((int) ceil(log10((num ? num : 1) + 1)) + 1);
    sprintf(str, "%d", num);
    return str;
}
