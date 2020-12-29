#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "linkedList.h"

typedef struct td_listNode *td_listNode;

struct td_linkedList {
    int length;
    td_listNode head;
    td_listNode tail;
};

struct td_listNode {
    char *key;
    void *data;
    void (*freeFunc)(void *);
    td_listNode prev;
    td_listNode next;
};

td_linkedList createLinkedList() {
    td_linkedList list = malloc(sizeof(struct td_linkedList));
    list -> head = NULL;
    list -> tail = NULL;
    list -> length = 0;
    return list;
}

void appendWithFree(td_linkedList list, void *data, char *key, void *freeFunc) {
    // Create a new node to store the data
    td_listNode node = malloc(sizeof(struct td_listNode));
    char *keyCopy = malloc(strlen(key) + 1);
    node -> key = strcpy(keyCopy, key);
    node -> data = data;
    node -> freeFunc = freeFunc;

    // Append the node to the list
    if(!list -> tail) {
        list -> head = node;
        node -> next = node;
        node -> prev = node;
    } else {
        td_listNode tail = list -> tail;
        tail -> next = node;
        node -> prev = tail;
        node -> next = list -> head;
        list -> head -> prev = node;
    }

    list -> tail = node;
    list -> length++;
}

void append(td_linkedList list, void *data, char *key) {
    appendWithFree(list, data, key, NULL);
}

void appendListCallback(void *elementData, void *callbackData, char *elementKey) {
    td_linkedList dest = (td_linkedList) callbackData;
    append(dest, elementData, elementKey);
}

void appendList(td_linkedList dest, td_linkedList src) {
    listForEach(src, appendListCallback, dest);
}

void dangerouslyAddFreeFunc(td_linkedList list, void *freeFunc) {
    if(list -> length == 0) return;
    td_listNode current = list -> head;

    do {
        current -> freeFunc = freeFunc;
        current = current -> next;
    } while(current != list -> head);
}

void *getFromList(td_linkedList list, char *key) {
    if(!list -> head) return NULL;

    td_listNode current = list -> head;

    do {
        if(strcmp(current -> key, key) == 0) return current -> data;
        current = current -> next;
    } while(current != list -> head);

    return NULL;
}

void listForEach(td_linkedList list, void (*callback)(void *, void*, char*), void *data) {
    if(list -> length == 0) return;
    td_listNode current = list -> head;

    do {
        callback(current -> data, data, current -> key);
        current = current -> next;
    } while(current != list -> head);
}

char *listToString(td_linkedList list) {
    if(list -> length == 0) {
        char *blank = malloc(3);
        strcpy(blank, "[]");
        return blank;
    }

    // Accumulate length of keys
    // TODO: Re-allocate the length of the string instead of traversing the list twice
    int accLength = 0;
    td_listNode current = list -> head;

    do {
        accLength += strlen(current -> key);
        current = current -> next;
    } while(current != list -> head);

    char *out = malloc(accLength + 4 * (list -> length - 1) + 1);
    size_t offset = 0;

    do {
        if(current -> next == list -> head) {
            sprintf(out + offset, "%s", current -> key);
        } else {
            sprintf(out + offset, "%s -> ", current -> key);
            offset += strlen(current -> key) + 4;
        }

        current = current -> next;
    } while(current != list -> head);

    return out;
}

int listLength(td_linkedList list) {
    return list -> length;
}

void destroyLinkedList(td_linkedList list) {
    if(list -> length == 0) return;

    td_listNode current = list -> head;

    do {
        if(current -> freeFunc) {
            current -> freeFunc(current -> data);
        }
        free(current -> key);
        td_listNode next = current -> next;
        free(current);
        current = next;
    } while(current != list -> head);

    free(list);
}