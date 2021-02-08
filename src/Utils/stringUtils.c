#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "stringUtils.h"

char *stringifyInt(unsigned int num) {
    char *str = malloc((int) ceil(log10((num ? num : 1) + 1)) + 1);
    sprintf(str, "%d", num);
    return str;
}
