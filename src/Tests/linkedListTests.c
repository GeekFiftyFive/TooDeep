#include "linkedListTests.h"
#include "testHelper.h"
#include "../DataStructures/LinkedList/linkedList.h"

int runLinkedListTests() {
    int failedTests = 0;

    // Create a linked list to test against
    td_linkedList list1 = createLinkedList();

    // Create some data
    int a = 2, b = 3, c = 4;

    // Append the data to the list
    append(list1, &a, "A");
    append(list1, &b, "B");
    append(list1, &c, "C");

    // Retrieve the data and assert the result
    failedTests += assert(a, *((int *) getFromList(list1, "A")));
    failedTests += assert(b, *((int *) getFromList(list1, "B")));
    failedTests += assert(c, *((int *) getFromList(list1, "C")));

    return failedTests;
}