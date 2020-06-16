#ifndef HASH_TABLE
#define HASH_TABLE

typedef struct td_hashMap *td_hashMap;

td_hashMap createHashMap(int);

td_hashMap createHashMapWithLoadFactor(int, float);

void insertIntoHashMap(td_hashMap, char *, void *, void *);

void *getFromHashMap(td_hashMap, char *);

void printHashMap(td_hashMap);

void destroyHashMap(td_hashMap);

#endif