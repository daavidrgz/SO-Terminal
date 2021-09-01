#include "linked_list.h"

void createNode (PosL* pos) {
    *pos = malloc(sizeof(**pos));
}

void createEmptyList(LinkedList *listHead){
    createNode(listHead);
    (*listHead)->next = NULL;
    strcpy((*listHead)->data, "");
}

void insertItemList (LinkedList listHead ,PosL pos,  char * command) {
    PosL q, r;

    createNode(&q);

    strcpy(q->data, command);
    q->next = NULL;
    if( pos == NULL ) {
        r = listHead;
        if (r->next == NULL) {
            r->next = q;
        } else {
            while(r->next != NULL) {
                r = r->next;
            }
            r->next = q;
        }
    }
}

void clearList(LinkedList listHead) {
    PosL iterator = listHead->next;
    PosL newIterator = NULL;

    while(iterator != NULL){
        newIterator = iterator->next;
        free(iterator);
        iterator = newIterator;
    }
    listHead->next = NULL;
}

char* getItem(LinkedList listHead, int itemNumber){
    PosL temp = listHead;
    int counter = 0;

    while ( temp->next != NULL && counter != itemNumber+1)  {
        temp = temp->next;
        counter++;
    }
    if(counter != itemNumber +1 ) {
        return NULL;
    }else {
        return temp->data;
    }


}
void printList(LinkedList listHead) {
    PosL temp = listHead->next;
    int counter = 0;
    while (temp != NULL) {
        printf("%d == > %s",counter, temp->data);
        counter++;
        temp = temp->next;
    }
}

void printFromNHead (LinkedList listHead, int itemNumber) {
    PosL temp = listHead->next;
    int counter = 0;
    while (temp != NULL && counter <= itemNumber) {
        printf("%d == > %s",counter, temp->data);
        counter++;
        temp = temp->next;
    }
}
