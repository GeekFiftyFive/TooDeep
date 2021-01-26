#include "stateMachineTests.h"
#include "testHelper.h"

int runStateMachineTests() {
    td_stateMachine machine = createStateMachine();
    td_stateMachineNode node = createStateMachineNode(machine, "start", true);
    addStateMachineIntVariable(machine, "count", 0);

    destroyStateMachine(machine);
    return 0;
}
