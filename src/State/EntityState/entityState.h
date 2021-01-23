#ifndef TD_ENTITY_STATE
#define TD_ENTITY_STATE

#include "../../Scripting/luaValues.h"

typedef struct td_entityState *td_entityState;

td_entityState createEntityState();

td_scriptValType entityStateCheckType(td_entityState, char *);

void insertIntoState(td_entityState, char *, td_scriptValType, td_scriptVal);

float getFloatFromState(td_entityState, char *);

int getIntFromState(td_entityState, char *);

bool getBoolFromState(td_entityState, char *);

char* getStringFromState(td_entityState, char *);

void destroyEntityState(td_entityState);

#endif
