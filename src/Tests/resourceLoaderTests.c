#include <stdlib.h>
#include "../IO/resourceLoader.h"
#include "resourceLoaderTests.h"
#include "testHelper.h"

#define INIT_TEXT "This is the initial plaintext"
#define FINAL_TEXT "This is the final plaintext"

char *initialPlaintextLoader(char *path) {
    return INIT_TEXT;
}

char *finalPlaintextLoader(char *path) {
    return FINAL_TEXT;
}

int runResourceLoaderTests() {
    int failedTests = 0;

    td_resourceLoader rl = createResourceLoader("/");

    setPlaintextLoader(rl, initialPlaintextLoader);

    char *actual = loadPlaintextResource(rl, "");
    assertString(INIT_TEXT, actual, "The initial text should be loaded");

    setPlaintextLoader(rl, finalPlaintextLoader);
    actual = loadPlaintextResource(rl, "");
    assertString(INIT_TEXT, actual, "The initial text should still be loaded");

    actual = loadPlaintextResource(rl, "new");
    assertString(FINAL_TEXT, actual, "The new text should now be loaded");

    destroyResourceLoader(rl);

    return failedTests;
}