#ifndef MEMORY_LIST_H
#define MEMORY_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>


#define MAX_STR 1024

typedef struct memNode* PosM;
struct memNode {
    void* pointer;
    size_t size;
    time_t time;
    char type[10];
    key_t key;
    char filename[MAX_STR];
    int fd;
    PosM next;
};
typedef PosM MemoryList;

void createEmptyMemList(MemoryList*);
bool isEmptyMemList(MemoryList);
bool insertItemMemList (MemoryList, PosM, struct memNode);
void clearMemList(MemoryList);
void printMemList(MemoryList, char*);
void deleteItemMemList(MemoryList, PosM);
PosM getSharedItem(MemoryList, key_t);
PosM getMmapItem(MemoryList, char*);
PosM getMallocItem(MemoryList, size_t);
PosM searchAddr(MemoryList, void*);

#endif
