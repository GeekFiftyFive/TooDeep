#include <stdlib.h>
#include <math.h>
#include "entityState.h"
#include "../../DataStructures/HashMap/hashMap.h"

struct td_entityState {
    td_hashMap store;
};

struct td_entry {
    td_scriptValType type;
    td_scriptVal value;
};

td_entityState createEntityState() {
    td_entityState state = malloc(sizeof(struct td_entityState));

    state -> store = createHashMap(20);

    return state;
}

void insertIntoState(td_entityState state, char *name, td_scriptValType type, td_scriptVal value) {
    struct td_entry *entry = getFromHashMap(state -> store, name);

    if(entry) {
        entry -> value = value;
        return;
    }

    entry = malloc(sizeof(struct td_entry));
    entry -> type = type;
    entry -> value = value;
    insertIntoHashMap(state -> store, name, entry, free);
}

td_scriptValType entityStateCheckType(td_entityState state, char *name) {
    struct td_entry *entry = getFromHashMap(state -> store, name);

    if(!entry) {
        return NIL;
    }

    return entry -> type;
}

float getFloatFromState(td_entityState state, char *name) {
    struct td_entry *entry = getFromHashMap(state -> store, name);
    if(entry -> type != FLOAT) {
        return NAN;
    }

    return entry -> value.floatVal;
}

int getIntFromState(td_entityState state, char *name) {
    struct td_entry *entry = getFromHashMap(state -> store, name);
    if(entry -> type != INT) {
        return 0;
    }

    return entry -> value.intVal;
}

bool getBoolFromState(td_entityState state, char *name) {
    struct td_entry *entry = getFromHashMap(state -> store, name);
    if(entry -> type != BOOL) {
        return false;
    }

    return entry -> value.booleanVal;
}

char* getStringFromState(td_entityState state, char *name) {
    struct td_entry *entry = getFromHashMap(state -> store, name);
    if(entry -> type != STRING) {
        return NULL;
    }

    return entry -> value.stringVal;
}

void destroyEntityState(td_entityState entityState) {
    destroyHashMap(entityState -> store);
    free(entityState);
}
