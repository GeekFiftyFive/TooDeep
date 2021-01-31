#include <stdlib.h>
#include <strings.h>
#include "stateMachine.h"
#include "../../DataStructures/LinkedList/linkedList.h"
#include "../../DataStructures/HashMap/hashMap.h"
#include "../../IO/logger.h"

#define checkCondition(x, y, op) switch(op) { case TD_EQ: return x == y; case TD_GT: return x > y; case TD_LT: return x < y; case TD_GTE: return x >= y; case TD_LTE: return x <= y; }

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
    bool isFloat;
};

td_stateMachine createStateMachine() {
    td_stateMachine stateMachine = malloc(sizeof(struct td_stateMachine));

    stateMachine -> nodes = createLinkedList();
    stateMachine -> variables = createHashMap(10);

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

    node -> connections = createLinkedList();

    appendWithFree(stateMachine -> nodes, node, idCpy, destroyStateMachineNode);

    return node;
}

td_stateMachineNode getStateMachineNode(td_stateMachine stateMachine, char *id) {
    return getFromList(stateMachine -> nodes, id);
}

td_stateMachineConnection addStateMachineConnection(td_stateMachineNode src, td_stateMachineNode dst) {
    td_stateMachineConnection connection = malloc(sizeof(struct td_stateMachineConnection));

    connection -> node = dst;
    connection -> conditions = createLinkedList();
    appendWithFree(src -> connections, connection, NULL, destroyStateMachineConnection);

    return connection;
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

bool isStateMachineVariableInt(td_stateMachine stateMachine, char *name) {
    struct td_stateMachineValueType *var = getFromHashMap(stateMachine -> variables, name);
    return !var -> isFloat;
}

bool isStateMachineVariableFloat(td_stateMachine stateMachine, char *name) {
    return !isStateMachineVariableInt(stateMachine, name);
}

static bool checkIntCondition(int actual, int target, td_stateMachineOperation op) {
    checkCondition(actual, target, op);
    return false;
}

static bool checkFloatCondition(float actual, float target, td_stateMachineOperation op) {
    checkCondition(actual, target, op);
    return false;
}

static void connectionCallback(void *entryData, void *callbackData, char *key) {
    td_stateMachineConnection connection = (td_stateMachineConnection) entryData;
    td_stateMachine stateMachine = (td_stateMachine) callbackData;
    td_iterator iterator = getIterator(connection -> conditions);
    struct td_stateMachineCondition *condition = NULL;
    bool followConnection = true;

    while((condition = (struct td_stateMachineCondition *) iteratorNext(iterator))) {
        struct td_stateMachineValueType *valueType =
            (struct td_stateMachineValueType *) getFromHashMap(stateMachine -> variables, condition -> varName);
        if(!valueType) {
            followConnection = false;
            logError("Could not find state machine variable with name %s\n", condition -> varName);
            break;
        }
        if(valueType -> isFloat) {
            followConnection = followConnection && checkFloatCondition(valueType -> value.fVal, condition -> value.fVal, condition -> operation);
        } else {
            followConnection = followConnection && checkIntCondition(valueType -> value.iVal, condition -> value.iVal, condition -> operation);
        }
    }

    if(followConnection) {
        stateMachine -> currentNode = connection -> node;
    }

    destroyIterator(iterator);
}

static void updateState(td_stateMachine stateMachine) {
    td_stateMachineNode node = stateMachine -> currentNode;
    listForEach(node -> connections, connectionCallback, stateMachine);
}

static void updateStateMachineCondition(td_stateMachine stateMachine, char *name, union td_stateMachineValue value) {
    struct td_stateMachineValueType *currentValue = getFromHashMap(stateMachine -> variables, name);

    currentValue -> value = value;
    updateState(stateMachine);
}

void updateStateMachineFloatCondition(td_stateMachine stateMachine, char *name, float value) {
    union td_stateMachineValue valueUnion;
    valueUnion.fVal = value;
    updateStateMachineCondition(stateMachine, name, valueUnion);
}

void updateStateMachineIntCondition(td_stateMachine stateMachine, char *name, int value) {
    union td_stateMachineValue valueUnion;
    valueUnion.iVal = value;
    updateStateMachineCondition(stateMachine, name, valueUnion);
}

char *getCurrentStateId(td_stateMachine stateMachine) {
    return stateMachine -> currentNode -> id;
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
