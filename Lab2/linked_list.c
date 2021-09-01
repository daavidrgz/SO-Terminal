#include "linked_list.h"

#define GREEN "\033[1;32m"
#define WHITE "\033[0m"

bool createNode(PosL* p){
    *p = malloc(sizeof(**p));
    return *p != NULL;
}

bool isEmptyList(LinkedList listHead){
    return listHead->next == NULL;
}

void createEmptyList(LinkedList *listHead){
    createNode(listHead);
    (*listHead)->next = NULL;
    strcpy((*listHead)->data, "");
}

bool insertItemList (LinkedList listHead ,PosL pos,  char * command) {
    PosL q, r;

    if(!createNode(&q)) {
        return false;
    } else {
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
    return true;
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

void printList(LinkedList listHead, int itemNumber){
    PosL p = listHead->next;
    int pos_list = 0;

    while(itemNumber != 0 && p != NULL){
        printf("%s%d%s-> %s", GREEN, pos_list, WHITE, p->data);
        p = p->next;
        itemNumber--;
        pos_list++;
    }
    if(itemNumber > 0){
        printf("There is no more elements in the history\n");
    }
}
