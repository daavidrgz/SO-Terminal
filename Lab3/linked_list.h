#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STR 1024

typedef struct node* PosL;
struct node {
    char data [MAX_STR];
    PosL next;
};
typedef PosL LinkedList;

void createEmptyList(LinkedList*);
bool isEmptyList(LinkedList);
bool insertItemList (LinkedList, PosL, char*);
void clearList(LinkedList);
char* getItem(LinkedList, int);
void printList(LinkedList, int);

#endif
