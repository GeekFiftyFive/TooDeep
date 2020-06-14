#include <stdlib.h>
#include "hashMap.h"
#include "linkedList.h"

#define DEFAULT_LOAD_FACTOR 0.8

struct td_hashMap {
    td_linkedList *entries;
    int size;
    float loadFactor;
};

td_hashMap createHashMap(int initCapacity) {
    return createHashMapWithLoadFactor(initCapacity, DEFAULT_LOAD_FACTOR);
}

td_hashMap createHashMapWithLoadFactor(int initCapacity, float loadFactor) {
    return NULL;
}

void insertIntoHashMap(td_hashMap map, char *key, void *data) {
    
}

void *getFromHashMap(td_hashMap map, char *key) {
    return NULL;
}