#ifndef LINKED_LIST
#define LINKED_LIST

typedef struct td_linkedList *td_linkedList;

td_linkedList createLinkedList();

void appendWithFree(td_linkedList, void *, char *, void *freeFunc);

void append(td_linkedList, void *, char *);

void *getFromList(td_linkedList, char *);

void listForEach(td_linkedList, void (*callback)(void *, void *), void *);

char *listToString(td_linkedList);

void destroyLinkedList(td_linkedList);

#endif