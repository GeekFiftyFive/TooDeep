#include "stateMachineTests.h"
#include "testHelper.h"
#include "../State/StateMachine/stateMachine.h"

int runStateMachineTests() {
    td_stateMachine machine = createStateMachine();

    addStateMachineIntVariable(machine, "count", 0);

    td_stateMachineNode start = createStateMachineNode(machine, "start", true);
    td_stateMachineNode end = createStateMachineNode(machine, "end", false);
    td_stateMachineConnection connection = addStateMachineConnection(start, end);
    td_stateMachineConnection connection2 = addStateMachineConnection(end, start);
    addStateMachineIntCondition(connection, "count", 5, TD_GT);
    addStateMachineIntCondition(connection2, "count", 1, TD_EQ);

    for(int i = 0; i <= 5; i++) {
        updateStateMachineIntCondition(machine, "count", i);
        char *state = getCurrentStateId(machine);
        assertString("start", state, "State machine condition not met");
    }

    updateStateMachineIntCondition(machine, "count", 6);
    char *state = getCurrentStateId(machine);
    assertString("end", state, "State machine condition met");

    updateStateMachineIntCondition(machine, "count", 1);
    state = getCurrentStateId(machine);
    assertString("start", state, "State machine condition met");

    destroyStateMachine(machine);
    return 0;
}
