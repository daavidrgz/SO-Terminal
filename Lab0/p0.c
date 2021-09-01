#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>

#include "linked_list.h"
#define STRMAX 1024

int historicArgumentType(char* argument);
int getArgumentNumericValue(char firstArgument[]);
int trocearCadena(char* cadena, char* trozos[]);
void printPresentWorkingDirecory();
void changeDirectory(char* argument);
void printPID();
void printPPID();
void printDate();
void printTime();
void printAuthors(char* argument);
void processCommand(LinkedList listHead, char * segmentedCommand[]);

int main() {
    int finished = 0;
    char * username = getenv("USER");
    char * segmentedCommand[STRMAX];
    LinkedList historic;

    createEmptyList(&historic);

    char lastCommand[STRMAX];
    char commandInit[STRMAX];
	char current_directory[STRMAX];

    do {
        lastCommand[0] = 0;
        getcwd(current_directory, STRMAX);
        printf("\033[1;35m");
        printf("%s",  username);
        printf("\033[0m");
        printf(":");
        printf("\033[1;36m");
        printf("%s", current_directory);
        printf("\033[0m");
        printf("> ");
        fgets( lastCommand, STRMAX, stdin);

        int commandLenght= strlen(lastCommand);
        if(commandLenght != 1){
            strcpy(commandInit, lastCommand);
            trocearCadena(lastCommand,segmentedCommand);
            if ( strcmp(lastCommand, "exit") == 0 ||
                    strcmp(lastCommand, "end") == 0 ||
                    strcmp(lastCommand, "quit") == 0)
            {
                finished = 1;
                clearList(historic);
                free(historic);
            } else {
                if (strcmp(lastCommand, "historic")==0
                        && historicArgumentType(segmentedCommand[1]) == 2){
                    processCommand(historic, segmentedCommand);
                }else {
                    insertItemList(historic,NULL, commandInit);
                    processCommand(historic, segmentedCommand);
                }

            }
        }
    } while ( finished == 0 );
}

int historicArgumentType (char* argument) {
    if (argument == NULL) {
        return 0;
    } else if (argument[0] == '-') {
        if (argument[1] == 'c'){
            return 1;
        } else if (argument[1] == 'r') {
            return 2;
        } else if ( argument[1] >= '0' && argument[1] <= '9' ) {
            return 3;
        }
    }
    return -1;
}


void printPresentWorkingDirecory() {
    char cwd[STRMAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }
}

void printPID() {
    printf("%ld\n", (long)getpid());
}

void printPPID() {
    printf("%ld\n", (long)getppid());
}

void printDate() {
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%d/%m/%y", tm_info);
    puts(buffer);
}

void printTime() {
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%H:%M:%S", tm_info);
    puts(buffer);
}

void printAuthors(char * argument) {

    if (argument != NULL ) {
        if (strcmp(argument, "-l") == 0) {
            printf("leopoldo.estevez\ndaviz.rbacelar\n");
        } else if (strcmp(argument, "-n") == 0)  {
            printf("Leopoldo Estevez\nDaviz RodrÃ­guez\n");
        }
    }else if(argument == NULL){
        printf("Leopoldo Estevez : leopoldo.estevez \nDaviz RodrÃ­guez: daviz.rbacelar \n");
    }else{
        printf("Argument %s not valid \n", argument);
    }
}

void changeDirectory(char * argument) {
    int correctExecution = chdir(argument);
    if (correctExecution == -1 ) {
        printf("%s\n", strerror(errno));
    }
}

int trocearCadena(char * cadena, char * trozos[])
{
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL) {
        return 0;
    }
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL) {
        i++;
    }
    return i;
}
void processCommand(LinkedList listHead, char * segmentedCommand[]) {
    if (strcmp(segmentedCommand[0], "pwd") == 0) {
        printPresentWorkingDirecory();

    } else if (strcmp(segmentedCommand[0], "getpid") == 0) {
        printPID();

    } else if (strcmp(segmentedCommand[0], "getppid") == 0) {
        printPPID();

    } else if (strcmp(segmentedCommand[0], "date") == 0) {
        printDate();

    } else if (strcmp(segmentedCommand[0], "time") == 0) {
        printTime();

    } else if (strcmp(segmentedCommand[0], "authors") == 0) {
        printAuthors( segmentedCommand [1] );

    } else if (strcmp(segmentedCommand[0], "chdir") == 0) {
        changeDirectory( segmentedCommand [1] );

    } else if (strcmp(segmentedCommand[0], "historic") == 0) {
        int historicArgument = historicArgumentType(segmentedCommand[1]);

        if ( historicArgument == 0) {
            printList(listHead);

        } else if (historicArgument == 1){
            clearList(listHead);

        } else if ( historicArgument == 2 ) {
            int argumentNumericValue = atoi(&segmentedCommand[1][2]);
            char * fetchedCommand = getItem(listHead, argumentNumericValue);

            if (fetchedCommand != NULL){
                char commandInit[STRMAX];
                strcpy(commandInit, fetchedCommand);
                trocearCadena(commandInit, segmentedCommand);
                processCommand(listHead, segmentedCommand);
            }else {

                printf("Index out of bounds\n");
            }
        } else if ( historicArgument == 3 ) {
            char *temp = segmentedCommand[1] + 1;
            int maxCounted = atoi(temp);
            printFromNHead(listHead, maxCounted);
        }
    } else {
        printf("%s no encontrado \n", segmentedCommand[0]);
    }
}

