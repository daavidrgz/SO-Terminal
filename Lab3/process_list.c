#include "process_list.h"
#include "signal_handler.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#define GREEN "\033[1;32m"
#define WHITE "\033[0m"
#define CYAN "\033[0;34m"
#define PINK "\033[0;33m"
#define YELLOW "\033[0;35m"

void createEmptyArrayList(ArrayList *L) {
    L->lastPos = LNULL;
}

bool isEmptyArrayList(ArrayList L) { return L.lastPos == LNULL; }

bool insertItemArrayList(arrayItem d, arrayPos p, ArrayList *L) {
    arrayPos i;
    
    if ( L -> lastPos == MAX - 1 ) {
        return false;
    } else {
        L -> lastPos++;
        if ( p == LNULL ) {
            L->data[L->lastPos] = d;
        } else {
            for ( i = L -> lastPos; i >= p + 1; i-- )
                L -> data[i] = L -> data[i - 1];
            L -> data[p] = d;
        }
        return true;
    }
}

void clearArrayList(ArrayList L) {
    deleteItemsArrayList(NULL, &L);
}

void deleteItemsArrayList(char* state, ArrayList* L) {
    int i = 0;

    while ( i <= L->lastPos ) {
        if ( state == NULL || strcmp(L->data[i].state, state) == 0 )
            deleteAtPositionArrayList(i, L);
        else
            i++;
    }
}

void deleteAtPositionArrayList(arrayPos p, ArrayList *L) {
    L -> lastPos--;
    for ( int i = p; i <= L -> lastPos; i++ )
        L -> data[i] = L -> data[i + 1];
}

arrayItem getItemArrayList(arrayPos p, ArrayList L) {
    return L.data[p];
}

arrayPos findItemArrayList(pid_t pid, ArrayList L) {
    arrayPos p;

    if ( isEmptyArrayList(L) )
        return LNULL;
    else {
        for ( p = 0; (p <= L.lastPos) && (pid != L.data[p].pid); p++ );
        if (p <= L.lastPos)
            return p;
        else
            return LNULL;
    }
}

void updateItems(ArrayList* L) {
    int i = 0;
    int signal;
    char buffer[50];

    while ( i <= L->lastPos ) {
        L->data[i].priority = getpriority(PRIO_PROCESS, L->data[i].pid);
        if ( waitpid(L->data[i].pid, &signal, WNOHANG | WUNTRACED | WCONTINUED) == L->data[i].pid ) {
            if ( WIFEXITED(signal) ) {
                sprintf(buffer, "%d", WEXITSTATUS(signal));
                strcpy(L->data[i].signal, buffer);
                strcpy(L->data[i].state, "TERMINATED NORMALLY");
            } else if ( WIFSIGNALED(signal) ) {
                strcpy(L->data[i].signal, sig2Str(WTERMSIG(signal)));
                strcpy(L->data[i].state, "TERMINATED BY SIGNAL");
            } else if ( WIFSTOPPED(signal) ) {
                strcpy(L->data[i].state, "STOPPED");
            } else if ( WIFCONTINUED(signal) ) {
                strcpy(L->data[i].state, "RUNNING");
            }
        }
        i++;
    }
}

void printArrayList(ArrayList* L, pid_t pid) {
    int i = 0;
    char time[100];

    updateItems(L);
    while ( i <= L->lastPos ) {
        if ( pid == L->data[i].pid || pid == -1 ) {
            strftime(time, 100, "%b %d %H:%M:%S", localtime(&L->data[i].time));
            printf("%s%3d%s-> ", GREEN, i, WHITE);
            printf("%s%6d%s  %s%3d%s   %s    %s%-20s%s   %4s   %-s\n", PINK, L->data[i].pid, WHITE, CYAN, L->data[i].priority, 
                    WHITE, time, YELLOW, L->data[i].state, WHITE, L->data[i].signal, L->data[i].command);
        }
        i++;
    }
}
