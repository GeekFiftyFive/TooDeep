#include "resourceLoader.h"
#include "../DataStructures/HashMap/hashMap.h"

struct td_resourceLoader {
    td_hashMap resources;
    char *basePath;
};