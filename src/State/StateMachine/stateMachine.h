#ifndef TD_STATE_MACHINE
#define TD_STATE_MACHINE

typedef enum {TD_EQ, TD_GT, TD_LT, TD_GTE, TD_LTE} td_stateMachineOperation;

typedef struct td_stateMachine *td_stateMachine;
typedef struct td_stateMachineNode *td_stateMachineNode;

td_stateMachine createStateMachine();

#endif
