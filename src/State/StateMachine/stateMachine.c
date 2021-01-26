#include <stdlib.h>
#include <strings.h>
#include "stateMachine.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"

struct td_stateMachine {
    td_stateMachineNode currentNode;
    td_linkedList nodes;
    td_hashMap variables;
};

struct td_stateMachineNode {
    char *id;
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
    char *varName;
    union td_stateMachineValue value;
    td_stateMachineOperation operation;
    bool isFloat;
};

struct td_stateMachineValueType {
    union td_stateMachineValue value;
    bool ifFloat;
};

td_stateMachine createStateMachine() {
    td_stateMachine stateMachine = malloc(sizeof(struct td_stateMachine));

    stateMachine -> nodes = createLinkedList();
    stateMachine -> variables = createHashMap();

    return stateMachine;
}

void destroyStateMachineNode(td_stateMachineNode node);
void destroyStateMachineConnection(td_stateMachineConnection connection);
void destroyStateMachineCondition(struct td_stateMachineCondition *condition);

td_stateMachineNode createStateMachineNode(td_stateMachine stateMachine, char *id, bool current) {
    td_stateMachineNode node = malloc(sizeof(struct td_stateMachineNode));
    char *idCpy = malloc(strlen(id) + 1);
    strcpy(idCpy, id);
    node -> id = idCpy;

    if(current) {
        stateMachine -> currentNode = node;
    }

    appendWithFree(stateMachine -> nodes, node, NULL, destroyStateMachineNode);

    return node;
}

void addStateMachineConnection(td_stateMachineNode src, td_stateMachineNode dst) {
    td_stateMachineConnection connection = malloc(sizeof(struct td_stateMachineConnection));

    connection -> node = dst;
    appendWithFree(src -> connections, connection, NULL, destroyStateMachineConnection);
}

static void addCondition(td_stateMachineConnection connection, char *varName, union td_stateMachineValue val, bool isFloat, td_stateMachineOperation op) {
    struct td_stateMachineCondition *condition = malloc(sizeof(struct td_stateMachineCondition));
    condition -> isFloat = isFloat;
    condition -> operation = op;
    condition -> value = val;

    char *varNameCpy = malloc(strlen(varName) + 1);
    strcpy(varNameCpy, varName);
    condition -> varName = varNameCpy;

    appendWithFree(connection -> conditions, condition, NULL, destroyStateMachineCondition);
}

void addStateMachineFloatVariable(td_stateMachine stateMachine, char *name, float value) {
    struct td_stateMachineValueType *valueType = malloc(sizeof(struct td_stateMachineValueType));
    valueType -> isFloat = true;
    
    union td_stateMachineValue val;
    val.fVal = value;
    valueType -> value = val;
    insertIntoHashMap(stateMachine -> variables, name, valueType, free);
}

void addStateMachineIntVariable(td_stateMachine stateMachine, char *name, int value) {
    struct td_stateMachineValueType *valueType = malloc(sizeof(struct td_stateMachineValueType));
    valueType -> isFloat = false;
    
    union td_stateMachineValue val;
    val.iVal = value;
    valueType -> value = val;
    insertIntoHashMap(stateMachine -> variables, name, valueType, free);
}

void addStateMachineFloatCondition(td_stateMachineConnection connection, char *varName, float value, td_stateMachineOperation op) {
    union td_stateMachineValue val;
    val.fVal = value;
    addCondition(connection, varName, val, true, op);
}

void addStateMachineIntCondition(td_stateMachineConnection connection, char *varName, int value, td_stateMachineOperation op) {
    union td_stateMachineValue val;
    val.iVal = value;
    addCondition(connection, varName, val, false, op);
}

void destroyStateMachineCondition(struct td_stateMachineCondition *condition) {
    free(condition -> varName);
    free(condition);
}

void destroyStateMachineConnection(td_stateMachineConnection connection) {
    destroyLinkedList(connection -> conditions);
    free(connection);
}

void destroyStateMachineNode(td_stateMachineNode node) {
    destroyLinkedList(node -> connections);
    free(node -> id);
    free(node);
}

void destroyStateMachine(td_stateMachine stateMachine) {
    destroyLinkedList(stateMachine -> nodes);
    destroyHashMap(stateMachine -> variables);
    free(stateMachine);
}
