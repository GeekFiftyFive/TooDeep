#include "linkedListTests.h"
#include "testHelper.h"
#include "../DataStructures/LinkedList/linkedList.h"

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

    return failedTests;
}