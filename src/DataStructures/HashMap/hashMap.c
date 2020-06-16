#include <stdlib.h>
#include "hashMap.h"
#include "../LinkedList/linkedList.h"

#define DEFAULT_LOAD_FACTOR 0.8

struct td_hashMap {
    td_linkedList *entries;
    int size;
    int usage;
    float loadFactor;
};

td_hashMap createHashMap(int initCapacity) {
    return createHashMapWithLoadFactor(initCapacity, DEFAULT_LOAD_FACTOR);
}

td_hashMap createHashMapWithLoadFactor(int initCapacity, float loadFactor) {
    td_hashMap map = malloc(sizeof(struct td_hashMap));

    map -> entries = malloc(initCapacity * sizeof(td_linkedList));
    map -> size = initCapacity;
    map -> loadFactor = loadFactor;
    map -> usage = 0;

    // Initialise every linkedList
    for(int i = 0; i < initCapacity; i++) {
        map -> entries[i] = createLinkedList();
    }

    return map;
}

int hash(char *key) {
    int hashAddress = 0;
    for (int i = 0; key[i]!='\0'; i++){
        hashAddress = key[i] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
    }

    return hashAddress;
}

void insertIntoHashMap(td_hashMap map, char *key, void *data, void *freeFunc) {
    // TODO: Resize the hash map if the load factor has been hit
    int location = hash(key) % map -> size;

    if(freeFunc)
        appendWithFree(map -> entries[location], data, key, freeFunc);
    else
        append(map -> entries[location], data, key);
}

void *getFromHashMap(td_hashMap map, char *key) {
    int location = hash(key) % map -> size;
    return getFromList(map -> entries[location], key);
}

void destroyHashMap(td_hashMap map) {
    for(int i = 0; i < map -> size; i++) {
        destroyLinkedList(map -> entries[i]);
    }

    free(map -> entries);
    free(map);
}