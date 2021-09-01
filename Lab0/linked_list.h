#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STR 1024

typedef struct Node* PosL;
struct Node {
    char data [MAX_STR];
    PosL next;
};
typedef PosL LinkedList;

void createEmptyList(LinkedList*);
void insertItemList (LinkedList,PosL, char*);
void clearList(LinkedList);
char* getItem(LinkedList, int);
void printList(LinkedList);
void printFromNHead(LinkedList, int);

#endif
