#include <stdlib.h>
#include <stdbool.h>
#include "stateMachine.h"
#include "../../DataStructures/LinkedList/linkedList.h"

struct td_stateMachine {
    td_stateMachineNode currentNode;
    td_linkedList nodes;
};

struct td_stateMachineNode {
    td_linkedList connections;
};

struct td_stateMachineConnection {
    td_stateMachineNode node;
    td_linkedList conditions;
};

union td_stateMachineValue {
    float fVal;
    int iVal;
};

struct td_stateMachineCondition {
    union td_stateMachineValue value;
    td_stateMachineOperation operation;
    bool isFloat;
};

td_stateMachine createStateMachine() {
    td_stateMachine stateMachine = malloc(sizeof(struct td_stateMachine));

    stateMachine -> nodes = createLinkedList();

    return stateMachine;
}

void destroyStateMachine(td_stateMachine stateMachine) {
    destroyLinkedList(stateMachine -> nodes);
    free(stateMachine);
}
