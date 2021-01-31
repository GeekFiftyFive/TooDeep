#include <string.h>
#include "stateMachineLoader.h"
#include "../../IO/logger.h"

struct createNodeCallbackData {
    td_stateMachine machine;
    char *defaultNode;
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

    return machine;
}
