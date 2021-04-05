#include "jsonCoreTests.h"
#include "testHelper.h"
#include "jsonTestHelper.h"
#include "../JSON/jsonCore.h"

int runjsonCoreTests() {
    td_json json = parseJSON(TEST_JSON);
    return 0;
}
