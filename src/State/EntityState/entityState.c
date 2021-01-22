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

    state -> store = createHashMap(10);

    return state;
}

void insertIntoState(td_entityState state, char *name, td_scriptValType type, td_scriptVal value) {
   struct td_entry *entry = malloc(sizeof(struct td_entry));
   entry -> type = type;
   entry -> value = value;
   insertIntoHashMap(state -> store, name, entry, free);
}

void destroyEntityState(td_entityState entityState) {
    destroyHashMap(entityState -> store);
    free(entityState);
}
