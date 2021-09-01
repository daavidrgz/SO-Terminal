#include "memory_list.h"

#define GREEN "\033[1;32m"
#define WHITE "\033[0m"
#define CYAN "\033[0;34m"
#define PINK "\033[0;33m"


bool createMemNode(PosM* p) {
    *p = malloc(sizeof(**p));
    return *p != NULL;
}

bool isEmptyMemList(MemoryList listHead) {
    return listHead->next == NULL;
}

void createEmptyMemList(MemoryList *listHead) {
    createMemNode(listHead);
	
    (*listHead)->next = NULL;
    (*listHead)->pointer = NULL;
	(*listHead)->size = 0;
	(*listHead)->time = 0;
	(*listHead)->key = 0;
	(*listHead)->fd = 0;
    strcpy((*listHead)->filename, "");
	strcpy((*listHead)->type, "");
	
}

bool insertItemMemList (MemoryList listHead , PosM pos,  struct memNode data) {
    PosM q, r;

    if( !createMemNode(&q) ) {
        return false;
    } else {
		q->pointer = data.pointer;
		q->size = data.size;
		q->time = data.time;
		q->key = data.key;
		q->fd = data.fd;
        strcpy(q->type, data.type);
		strcpy(q->filename, data.filename);
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

void clearMemList(MemoryList listHead) {
    PosM iterator = listHead->next;
    PosM newIterator = NULL;

    while(iterator != NULL){
        newIterator = iterator->next;
		if ( strcmp(iterator->type, "malloc") == 0 ) {
			free(iterator->pointer);
		} else if ( strcmp(iterator->type, "mmap") == 0 ) {
			close(iterator->fd);
			munmap(iterator->pointer, iterator->size);
		} else {
			shmdt(iterator->pointer);
		}
        free(iterator);
        iterator = newIterator;
    }
    listHead->next = NULL;
}

void deleteItemMemList(MemoryList listHead, PosM p) {
	PosM tmp = listHead;
	while ( tmp->next != p )
		tmp = tmp->next;
	tmp->next = p->next;
	p->next = NULL;
	free(p);
}

PosM getMallocItem(MemoryList listHead, size_t size) {
	PosM tmp = listHead->next;
	bool end = false;

	while ( tmp != NULL && !end ) {
		if ( tmp->size == size && strcmp(tmp->type, "malloc") == 0 ) {
			end = true;
		} else
        	tmp = tmp->next; 
	}

    return tmp;
}

PosM getMmapItem(MemoryList listHead, char* filename) {
	PosM tmp = listHead->next;
	bool end = false;

	while ( tmp != NULL && !end ) {
		if ( strcmp(tmp->filename, filename) == 0 && strcmp(tmp->type, "mmap") == 0 ) {
			end = true;
		} else
        	tmp = tmp->next; 
	}

	return tmp;
}

PosM getSharedItem(MemoryList listHead, key_t key) {
	PosM tmp = listHead->next;
	bool end = false;

	while ( tmp != NULL && !end ) {
		if ( tmp->key == key && strcmp(tmp->type, "shared") == 0 ) {
			end = true;
		} else
        	tmp = tmp->next;
	}

	return tmp;
}

PosM searchAddr(MemoryList listHead, void* dir) {
	PosM tmp = listHead->next;
	bool end = false;

	while ( tmp != NULL && !end ) {
		if ( tmp->pointer == dir ) {
			end = true;
		} else
        	tmp = tmp->next;
	}

	return tmp;
}

void unitSizeMem(float size, char unitsize[MAX_STR]) {
    int i = 0;
    while ( (i < 3) && (size / 1024 > 1) ) {
        size = size / 1024;
        i++;
    }
    sprintf(unitsize, "%.2f", size);
    if ( i == 0 ) {
        strcat(unitsize, " B");
    } else if ( i == 1 ) {
        strcat(unitsize, " KB");
    } else if ( i == 2 ) {
        strcat(unitsize, " MB");
    } else if ( i == 3 ) {
        strcat(unitsize, " GB");
    }
}

void printMemList(MemoryList listHead, char* type) {
    PosM p = listHead->next;
    int pos_list = 0;
	char time[100];
	char size[10];

    while ( p != NULL ) {
		if ( type == NULL || strcmp(type, p->type) == 0 ) {
			unitSizeMem(p->size, size);
			strftime(time, 100, "%b %d %H:%M:%S", localtime(&p->time));

			printf("%s%d%s-> %p   %s%10s%s   %s   %s%s", GREEN, pos_list, WHITE,
					p->pointer, PINK, size, WHITE, time, CYAN, p->type);

			if ( strcmp(p->type, "mmap") == 0 )
				printf(" - %s (FD: %d)%s\n", p->filename, p->fd, WHITE);

			else if ( strcmp(p->type, "shared") == 0 )
				printf(" - (Key: %d)%s\n", p->key, WHITE);

			else
				printf("\n");
			
		}

        p = p->next;
        pos_list++;
    }
}

