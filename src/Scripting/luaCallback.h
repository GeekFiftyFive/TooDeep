#ifndef TD_LUA_CALLBACK
#define TD_LUA_CALLBACK

typedef struct td_luaCallback *td_luaCallback;

td_luaCallback createLuaCallback(int, int, float);

int getLuaCallbackReference(td_luaCallback);

int shouldTriggerLuaCallback(td_luaCallback, int);

void destroyLuaCallback(td_luaCallback);

#endif