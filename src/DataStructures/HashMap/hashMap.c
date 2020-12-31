#include <stdlib.h>
#include "hashMap.h"
#include "../../IO/logger.h"

#define DEFAULT_LOAD_FACTOR 0.8

struct td_hashMap {
    td_linkedList *entries;
    td_linkedList insertions;
    int size;
    int usage;
    float loadFactor;
};

struct dataWithFree {
    void *data;
    void (*freeFunc)(void *);
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

    map -> insertions = createLinkedList();

    return map;
}

int hash(char *key) {
    int hashAddress = 0;
    for (int i = 0; key[i] != '\0'; i++){
        hashAddress = key[i] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
    }

    return hashAddress > 0 ? hashAddress : -hashAddress;
}

void addToNewTable(void *nodeData, void *accData, char *key) {
    td_hashMap map = (td_hashMap) accData;
    struct dataWithFree *pair = (struct dataWithFree*) nodeData;
    insertIntoHashMap(map, key, pair -> data, pair -> freeFunc);
}

void insertIntoHashMap(td_hashMap map, char *key, void *data, void *freeFunc) {
    int location = hash(key) % map -> size;

    if(freeFunc)
        appendWithFree(map -> entries[location], data, key, freeFunc);
    else
        append(map -> entries[location], data, key);
    
    struct dataWithFree *pair = malloc(sizeof(struct dataWithFree));
    pair -> data = data;
    pair -> freeFunc = freeFunc;
    appendWithFree(map -> insertions, pair, key, free);

    map -> usage++;

    if((float) map -> usage / map -> size > map -> loadFactor) {
        // TODO: Do this in a less dumb way
        td_hashMap newMap = createHashMapWithLoadFactor(map -> size * 2, map -> loadFactor);
        listForEach(map -> insertions, addToNewTable, newMap);
        for(int i = 0; i < map -> size; i++) destroyLinkedList(map -> entries[i]);
        map -> size *= 2;
        map -> entries = newMap -> entries;
        destroyLinkedList(newMap -> insertions);
        free(newMap);
    }
}

void *getFromHashMap(td_hashMap map, char *key) {
    int location = hash(key) % map -> size;
    return getFromList(map -> entries[location], key);
}

td_linkedList getHashesMatching(td_hashMap map, char *key) {
    int location = hash(key) % map -> size;
    return map -> entries[location];
}

void printHashMap(td_hashMap map) {
    logInfo("HashMap: Usage %d, Size %d [\n", map -> usage, map -> size);
    for(int i = 0; i < map -> size; i++) {
        char *listString = listToString(map -> entries[i]);
        logInfo("%s\n", listString);
        free(listString);
    }
    logInfo("]\n");
}

void destroyHashMap(td_hashMap map) {
    for(int i = 0; i < map -> size; i++) {
        destroyLinkedList(map -> entries[i]);
    }

    destroyLinkedList(map -> insertions);
    free(map -> entries);
    free(map);
}