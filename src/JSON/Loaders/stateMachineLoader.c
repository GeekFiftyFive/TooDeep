#include <string.h>
#include "stateMachineLoader.h"
#include "../../IO/logger.h"

#define mapOp(input, expected, type) if(strcmp(input, expected) == 0) return type

struct createNodeCallbackData {
    td_stateMachine machine;
    char *defaultNode;
};

struct createConditionCallbackData {
    td_stateMachine machine;
    td_stateMachineConnection connection;
};

void createNodeCallback(td_json json, void *callbackData) {
    td_stateMachine machine = ((struct createNodeCallbackData*) callbackData) -> machine;
    char *defaultNode = ((struct createNodeCallbackData*) callbackData) -> defaultNode;
    td_jsonError error = JSON_NO_ERROR;
    char *name = getJSONString(json, NULL, &error);

    if(error == JSON_ERROR) {
        logWarn("Invalid animation! Animation must be specified by their id!\n");
        return;
    }

    bool isDefault = strcmp(defaultNode, name) == 0;
    createStateMachineNode(machine, name, isDefault);
}

td_stateMachineOperation getOpType(char *opName) {
    mapOp(opName, "=", TD_EQ);
    mapOp(opName, ">", TD_GT);
    mapOp(opName, "<", TD_LT);
    mapOp(opName, ">=", TD_GTE);
    mapOp(opName, "<=", TD_LTE);

    logError("Invalid comparison type: %s\n", opName);
    return TD_EQ;
}

void createConditionsCallback(td_json json, void *callbackData) {
    td_stateMachine machine = ((struct createConditionCallbackData*) callbackData) -> machine;
    td_stateMachineConnection connection = ((struct createConditionCallbackData*) callbackData) -> connection;

    td_jsonError error = JSON_NO_ERROR;
    char *opName = getJSONString(json, "comparison", &error);

    if(error == JSON_ERROR) {
        logError("Must specify comparison type for condition!\n");
        return;
    }

    td_stateMachineOperation op = getOpType(opName);

    char *var = getJSONString(json, "variable", &error);
    if(error == JSON_ERROR) {
        logError("Must specify variable name for condition!\n");
        return;
    }

    if(!jsonFieldExists(json, "value")) {
        logError("Must define value inside of condition!\n");
        return;
    }

    if(isStateMachineVariableInt(machine, var)) {
        int value;
        if(isJSONFieldBoolean(json, "value")) {
            value = getJSONBool(json, "value", &error);
        } else {
            value = getJSONInt(json, "value", &error);
        }

        if(error == JSON_ERROR) {
            logError("Invald value for state machine condition!\n");
            return;
        }

        addStateMachineIntCondition(connection, var, value, op);
    } else if(isStateMachineVariableFloat(machine, var)) {
        float value = getJSONDouble(json, "value", &error);
        
        if(error == JSON_ERROR) {
            logError("Invald value for state machine condition!\n");
            return;
        }
        
        addStateMachineFloatCondition(connection, var, value, op);
    } else {
        logError("How did I get here?\n");
    }
}

void createConnectionCallback(td_json json, void *callbackData) {
    td_stateMachine machine = (td_stateMachine) callbackData;

    // Get names of nodes
    td_jsonError error = JSON_NO_ERROR;
    char *startName = getJSONString(json, "start", &error);
    char *endName = getJSONString(json, "end", &error);
    if(error == JSON_ERROR) {
        logError("Must specify start and end nodes for state machine connections\n");
        return;
    }

    td_stateMachineNode start = getStateMachineNode(machine, startName);
    td_stateMachineNode end = getStateMachineNode(machine, endName);

    if(!start || !end) {
        logError("State machine nodes must be pre-defined in the 'animations' field\n");
    }

    td_stateMachineConnection connection = addStateMachineConnection(start, end);

    struct createConditionCallbackData conditionCallbackData = {
        machine,
        connection
    };

    jsonArrayForEach(json, "conditions", createConditionsCallback, &conditionCallbackData);
}

void createVariablesCallback(td_json json, void *callbackData) {
    td_stateMachine machine = (td_stateMachine) callbackData;

    td_jsonError error = JSON_NO_ERROR;
    char *name = getJSONString(json, "name", &error);
    if(error == JSON_ERROR) {
        logError("Must define variable name\n");
        return;
    }

    char *type = getJSONString(json, "type", &error);
    if(error == JSON_ERROR) {
        logError("Must define variable type\n");
        return;
    }

    if(!jsonFieldExists(json, "start_value")) {
        logError("Must define variable value!\n");
        return;
    }

    if(strcmp(type, "boolean") == 0) {
        bool value = getJSONBool(json, "start_value", &error);
        addStateMachineIntVariable(machine, name, value);
    } else if(strcmp(type, "integer") == 0) {
        int value = getJSONInt(json, "start_value", &error);
        addStateMachineIntVariable(machine, name, value);
    } else if(strcmp(type, "float") == 0) {
        float value = (float) getJSONDouble(json, "start_value", &error);
        addStateMachineFloatVariable(machine, name, value);
    } else {
        logError("Invalid variables type %s\n", type);
    }
}

td_stateMachine loadStateMachineFromJSON(td_json json) {
    td_stateMachine machine = createStateMachine();

    // Create node for each animation
    td_jsonError error = JSON_NO_ERROR;
    char *defaultNode = getJSONString(json, "default_animation", &error);

    if(error == JSON_ERROR) {
        destroyStateMachine(machine);
        logError("Could not load state machine. Default animation not set!\n");
        return NULL;
    }

    struct createNodeCallbackData nodeCallbackData = {
        machine,
        defaultNode
    };
    jsonArrayForEach(json, "animations", createNodeCallback, &nodeCallbackData);

    // Create variables
    jsonArrayForEach(json, "variables", createVariablesCallback, machine);

    // Create connections
    jsonArrayForEach(json, "connections", createConnectionCallback, machine);

    return machine;
}
