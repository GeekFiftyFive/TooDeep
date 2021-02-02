#include <stdlib.h>
#include <lualib.h>
#include <lauxlib.h>
#include "eventAttributes.h"
#include "../DataStructures/LinkedList/linkedList.h"

struct td_eventAttributes {
    td_linkedList variables;
};

struct td_attribute {
    td_scriptValType valType;
    td_scriptVal val;
};

td_eventAttributes createEventAttributes() {
    td_eventAttributes eventAttributes = malloc(sizeof(struct td_eventAttributes));

    eventAttributes -> variables = createLinkedList();

    return eventAttributes;
}

void registerEventAttribute(td_eventAttributes eventAttributes, td_scriptValType valType, td_scriptVal val, char *name) {
    struct td_attribute *attribute = malloc(sizeof(struct td_attribute));

    attribute -> valType = valType;
    attribute -> val = val;

    appendWithFree(eventAttributes -> variables, attribute, name, free);
}

void mapValueCallback(void *entryData, void *callbackData, char *key) {

}

void mapToLuaTable(td_eventAttributes eventAttributes, lua_State *state) {
    listForEach(eventAttributes -> variables, mapValueCallback, state);
}

void destroyEventAttributes(td_eventAttributes eventAttributes) {
    destroyLinkedList(eventAttributes -> variables);
    free(eventAttributes);
}
