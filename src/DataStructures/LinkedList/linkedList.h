#ifndef LINKED_LIST
#define LINKED_LIST

typedef struct td_linkedList *td_linkedList;

td_linkedList createLinkedList();

void appendWithFree(td_linkedList, void *, char *, void *freeFunc);

void append(td_linkedList, void *, char *);

void *getFromList(td_linkedList, char *);

void destroyLinkedList(td_linkedList);

#endif