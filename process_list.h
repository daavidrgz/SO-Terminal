#ifndef STATIC_LIST_H
#define STATIC_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define LNULL -1
#define MAX 100

typedef int arrayPos;

typedef struct {
    pid_t pid;
    int priority;
    char command[MAX];
    time_t time;
    char signal[MAX];
    char state[MAX];
}arrayItem;

typedef struct {
    arrayItem data[MAX];
    arrayPos lastPos;
}ArrayList;

void createEmptyArrayList(ArrayList*);
bool insertItemArrayList(arrayItem, arrayPos, ArrayList*);
void deleteAtPositionArrayList(arrayPos, ArrayList*);
void deleteItemsArrayList(char* state, ArrayList*);
void clearArrayList(ArrayList L);
arrayPos findItemArrayList(pid_t, ArrayList);
bool isEmptyArrayList(ArrayList);
arrayItem getItemArrayList(arrayPos, ArrayList);
void printArrayList(ArrayList* L, pid_t pid);

#endif
 
