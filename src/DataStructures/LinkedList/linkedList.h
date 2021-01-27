#ifndef LINKED_LIST
#define LINKED_LIST

typedef struct td_linkedList *td_linkedList;

typedef struct td_iterator *td_iterator;

td_linkedList createLinkedList();

void appendWithFree(td_linkedList, void *, char *, void *);

void append(td_linkedList, void *, char *);

void appendList(td_linkedList, td_linkedList);

void removeFromListMatchPointer(td_linkedList, void *);

void removeFromList(td_linkedList, char *);

void dangerouslyAddFreeFunc(td_linkedList, void *);

void *getFromList(td_linkedList, char *);

void listForEach(td_linkedList, void (*callback)(void *, void *, char *), void *);

char *listToString(td_linkedList);

int listLength(td_linkedList);

td_iterator getIterator(td_linkedList);

void *iteratorNext(td_iterator);

void destroyIterator(td_iterator);

void destroyLinkedList(td_linkedList);

#endif