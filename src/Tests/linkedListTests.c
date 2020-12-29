#include <stdlib.h>
#include "linkedListTests.h"
#include "testHelper.h"
#include "../DataStructures/LinkedList/linkedList.h"

#define EXPECTED_STRING "A -> B -> C"
#define CONCAT_EXPECTED_STRING "A -> B -> C -> D -> E -> F"

void testCallback(void *nodeData, void *accData, char *key) {
    // Dereference data
    int toAdd = *((int *) nodeData);
    int acc = *((int *) accData);
    acc += toAdd;

    // Set new accumulator value
    *((int *) accData) = acc;
}

int runLinkedListTests() {
    int failedTests = 0;

    // Create a linked list to test against
    td_linkedList list = createLinkedList();

    // Create some data
    int a = 2, b = 3, c = 4;

    // Append the data to the list
    append(list, &a, "A");
    append(list, &b, "B");
    append(list, &c, "C");

    // Retrieve the data and assert the result
    failedTests += assert(a, *((int *) getFromList(list, "A")), "Fetch 'A' from list");
    failedTests += assert(b, *((int *) getFromList(list, "B")), "Fetch 'B' from list");
    failedTests += assert(c, *((int *) getFromList(list, "C")), "Fetch 'C' from list");

    td_linkedList concatinated = createLinkedList();
    td_linkedList secondaryList = createLinkedList();

    // Create more data
    int d = 5, e = 6, f = 7;

    append(secondaryList, &d, "D");
    append(secondaryList, &e, "E");
    append(secondaryList, &f, "F");

    appendList(concatinated, list);
    appendList(concatinated, secondaryList);

    // Ensure that they are in the correct order
    char *stringList = listToString(list);
    failedTests += assertString(EXPECTED_STRING, stringList, "listToString");

    stringList = listToString(concatinated);
    failedTests += assertString(CONCAT_EXPECTED_STRING, stringList, "appendList");
    free(stringList);

    // Ensure that forEach function gets called on each list element
    int acc = 0;
    listForEach(list, testCallback, &acc);
    failedTests += assert(a + b + c, acc, "listForEach");
    
    // Test the length of the list
    failedTests += assert(3, listLength(list), "listLength");

    // Destroy the lists
    destroyLinkedList(list);
    destroyLinkedList(concatinated);
    destroyLinkedList(secondaryList);

    return failedTests;
}