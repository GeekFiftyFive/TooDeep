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

static void mapValueCallback(void *entryData, void *callbackData, char *key) {
    struct td_attribute *attribute = (struct td_attribute *) entryData;
    lua_State *state = (lua_State*) callbackData;

    lua_pushstring(state, key);

    if(attribute -> valType == INT) {
        lua_pushinteger(state, attribute -> val.intVal);
    } else if(attribute -> valType == FLOAT) {
        lua_pushnumber(state, attribute -> val.floatVal);
    } else if(attribute -> valType == BOOL) {
        lua_pushboolean(state, attribute -> val.booleanVal);
    } else if(attribute -> valType == STRING) {
        lua_pushstring(state, attribute -> val.stringVal);
    }

    lua_settable(state, -3);
}

void mapToLuaTable(td_eventAttributes eventAttributes, lua_State *state) {
    if(!listLength(eventAttributes -> variables)) {
        return;
    }
    lua_createtable(state, 0, listLength(eventAttributes -> variables));
    listForEach(eventAttributes -> variables, mapValueCallback, state);
    lua_setglobal(state, "eventAttributes");
}

void destroyEventAttributes(td_eventAttributes eventAttributes) {
    destroyLinkedList(eventAttributes -> variables);
    free(eventAttributes);
}
