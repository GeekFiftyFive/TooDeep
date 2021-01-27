#ifndef TD_STATE_MACHINE
#define TD_STATE_MACHINE

#include <stdbool.h>

typedef enum {TD_EQ, TD_GT, TD_LT, TD_GTE, TD_LTE} td_stateMachineOperation;

typedef struct td_stateMachine *td_stateMachine;
typedef struct td_stateMachineNode *td_stateMachineNode;
typedef struct td_stateMachineConnection *td_stateMachineConnection;

td_stateMachine createStateMachine();

td_stateMachineNode createStateMachineNode(td_stateMachine, char *, bool);

void addStateMachineFloatVariable(td_stateMachine, char *, float);

void addStateMachineIntVariable(td_stateMachine, char *, int);

td_stateMachineConnection addStateMachineConnection(td_stateMachineNode, td_stateMachineNode);

void addStateMachineFloatCondition(td_stateMachineConnection, char *, float, td_stateMachineOperation);

void addStateMachineIntCondition(td_stateMachineConnection, char *, int, td_stateMachineOperation);

void updateStateMachineFloatCondition(td_stateMachine, char *, float);

void updateStateMachineIntCondition(td_stateMachine, char *, int);

char *getCurrentStateId(td_stateMachine);

void destroyStateMachine(td_stateMachine);

#endif
