#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "stringUtils.h"

char *stringifyInt(int num) {
    char *str = malloc((int) ceil(log10(num)) + 1);
    sprintf(str, "%d", num);
    return str;
}
