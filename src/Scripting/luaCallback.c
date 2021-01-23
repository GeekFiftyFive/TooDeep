#include <stdlib.h>
#include "luaCallback.h"

struct td_luaCallback {
    int reference;
    int timeout;
    int creationTime;
};

td_luaCallback createLuaCallback(int reference, int creationTime, float seconds) {
    td_luaCallback callback = malloc(sizeof(struct td_luaCallback));

    callback -> reference = reference;
    callback -> timeout = 1000 * seconds;
    callback -> creationTime = creationTime;

    return callback;
}

int getLuaCallbackReference(td_luaCallback callback) {
    return callback -> reference;
}

int shouldTriggerLuaCallback(td_luaCallback callback, int time) {
    return time - callback -> creationTime > callback -> timeout;
}

void destroyLuaCallback(td_luaCallback callback) {
    free(callback);
}
