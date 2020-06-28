#include <stdlib.h>
#include <stdio.h>
#include "hashMapTests.h"
#include "../DataStructures/HashMap/hashMap.h"
#include "testHelper.h"

int runHashMapTests() {
    int failedTests = 0;

    // Instantaite the HashMap
    td_hashMap map = createHashMap(3);

    // Create data
    int a = 3;
    int b = 5;
    int c = 8;
    int d = 13;
    int e = 21;

    // Insert data into the HashMap
    insertIntoHashMap(map, "A", &a, NULL);
    insertIntoHashMap(map, "B", &b, NULL);
    insertIntoHashMap(map, "C", &c, NULL);
    insertIntoHashMap(map, "D", &d, NULL);
    insertIntoHashMap(map, "E", &e, NULL);

    // Assert that the values can be retrieved
    failedTests += assert(a, *((int *) getFromHashMap(map, "A")), "Retrieve value A from HashMap");
    failedTests += assert(b, *((int *) getFromHashMap(map, "B")), "Retrieve value B from HashMap");
    failedTests += assert(c, *((int *) getFromHashMap(map, "C")), "Retrieve value C from HashMap");
    failedTests += assert(d, *((int *) getFromHashMap(map, "D")), "Retrieve value D from HashMap");
    failedTests += assert(e, *((int *) getFromHashMap(map, "E")), "Retrieve value E from HashMap");

    // Assert that NULL is returned for a non-existant value
    failedTests += assert(NULL, getFromHashMap(map, "INVALID"), "Retrieve a non existant value from HashMap");

    // Destroy the HashMap
    destroyHashMap(map);

    return failedTests;
}