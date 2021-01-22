#ifndef TD_ENTITY_STATE
#define TD_ENTITY_STATE

#include "../../Scripting/luaFunctions.h"

typedef struct td_entityState *td_entityState;

td_entityState createEntityState();

void insertIntoState(td_entityState, char *, td_scriptValType, td_scriptVal);

void destroyEntityState(td_entityState);

#endif
