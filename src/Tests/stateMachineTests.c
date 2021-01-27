#include "stateMachineTests.h"
#include "testHelper.h"
#include "../State/StateMachine/stateMachine.h"

int runStateMachineTests() {
    td_stateMachine machine = createStateMachine();

    addStateMachineIntVariable(machine, "count", 0);
    addStateMachineIntVariable(machine, "float", 0);

    td_stateMachineNode start = createStateMachineNode(machine, "start", true);
    td_stateMachineNode end = createStateMachineNode(machine, "end", false);
    td_stateMachineNode middle = createStateMachineNode(machine, "middle", false);
    td_stateMachineConnection connection = addStateMachineConnection(start, end);
    td_stateMachineConnection connection2 = addStateMachineConnection(end, start);
    td_stateMachineConnection connection3 = addStateMachineConnection(start, middle);
    addStateMachineIntCondition(connection, "count", 5, TD_GT);
    addStateMachineIntCondition(connection2, "count", 1, TD_EQ);
    addStateMachineFloatCondition(connection3, "float", 10.5, TD_GT);

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

    updateStateMachineFloatCondition(machine, "float", 10.6);
    state = getCurrentStateId(machine);
    assertString("middle", state, "State machine condition met");

    destroyStateMachine(machine);
    return 0;
}
