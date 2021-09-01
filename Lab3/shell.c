#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "linked_list.h"
#include "memory_list.h"
#include "process_list.h"
#include "signal_handler.h"

#define STRMAX 1024
#define LEERCOMPLETO ((ssize_t)-1)

#define GREEN "\033[1;32m"
#define WHITE "\033[0m"
#define CYAN "\033[1;34m"
#define PINK "\033[0;33m"
#define BLUE "\033[0;36m"
#define RED "\033[0;31m"
#define YELLOW "\033[1;35m"

MemoryList memList;
LinkedList historic;
int global1 = 5;
char global2 = 's';
float global3 = 5.6;

//Argument types
int historicArgumentType(char* segmentedCommand[]);
int createArgumentType(char* segmentedCommand[]);
int deleteArgumentType(char* segmentedCommand[]);
int memoryArgumentType(char* segmentedCommand[]);
int listArgumentType(char* segmentedCommand[], bool arguments[5]);
int getArgumentNumericValue(char firstArgument[]);

//System prints and historic
void printPWD();
void changeDirectory(char* segmentedCommand[]);
void manageHistoric(ArrayList* processList, char* segmentedCommand[]);
void printPID();
void printPPID();
void printDate();
void printTime();
void printAuthors(char* segmentedCommand[]);

//Create, delete and list files
void createDirFile(char* segmentedCommand[]);
void deleteDirFile(char* segmentedCommand[]);
void listDirFile(char* segmentedCommand[]);
void deleteRecursive(char* path);
void printPath(char* path, bool islong, bool ishide, bool isrec, int count);
void filePrint(char* path, char* name, bool islong);

//Memory managment
void memoryOperation(char* segmentedCommand[]);
void memoryAllocate(char* segmentedCommand[]);
void memoryDeallocate(char* segmentedCommand[]);
void memoryShow(char* segmentedCommand[]);
void memoryDeleteKey(char* key);
void memoryPmap();
void memoryShowVarFun(char* segmentedCommand[]);
void memDump(char* segmentedCommand[]);
void memFill(char* segmentedCommand[]);
void readFile(char* segmentedCommand[]);
void writeFile(char* segmentedCommand[]);
void doRecursive(char* segmentedCommand[]);
void doRecursiveAux(int n);

//Memory allocation
void memoryAllocateMmap(char* segmentedCommand[]);
void memoryAllocateMalloc(char* segmentedCommand[]);
void memoryAllocateCreateShared(char* segmentedCommand[]);
void memoryAllocateShared(char* segmentedCommand[]);

//Memory deallocation
void memoryDeallocMalloc(char* segmentedCommand[]);
void memoryDeallocMmap(char* segmentedCommand[]);
void memoryDeallocShared(char* segmentedCommand[]);
void memoryDeallocAddr(char* segmentedCommand[]);

//Processes
void getPriority(char* segmentedCommand[]);
void setPriority(char* segmentedCommand[]);
void getUid();
int setUid(char* sgementedCommand[]);
void forkProcess();
void executeCommand(char* segmentedCommand[]);
bool managePriority(char* segmentedCommand[], char* priority);
void executeForeground(char* segmentedCommand[]);
void executeBackground(char* segmentedCommand[], ArrayList* processList);
void listProcs();
void procInfo(char* segmentedCommand[], ArrayList* processList);
void deleteProcs(char* segmentedCommand[], ArrayList* processList);
void runAs(char* segmentedCommand[], ArrayList* processList);
void executeAs(char* segmentedCommand[], ArrayList* processList);
void executeProgram(char* segmentedCommand[], ArrayList* processList);


//Utilities
char* ConvierteModo(mode_t m, char *permisos);
char LetraTF(mode_t m);
char* getUser(uid_t uid);
char* getGroup(uid_t uid);
uid_t userUid(char* user);
void unitsize(float size, char unitsize[STRMAX]);
void cleanCommand(char* segmentedCommand[], char* clanedCommand[]);
int trocearCadena(char* cadena, char* trozos[]);
void processCommand(ArrayList* processList, char * segmentedCommand[]);
void shellHelp();
void memoryHelp();
void processHelp();
void fileSysHelp();
void utilitiesHelp();


int main() {
    createEmptyMemList(&memList);
    createEmptyList(&historic);
    bool finished = false;
    char* username = getenv("USER");
    char* segmentedCommand[STRMAX];
    ArrayList processList;
    char host[STRMAX];
    char lastCommand[STRMAX];
    char commandInit[STRMAX];
    char current_directory[STRMAX];

    createEmptyArrayList(&processList);
    gethostname(host, STRMAX);

    do {
        lastCommand[0] = 0;
        getcwd(current_directory, STRMAX);
        printf("%s%s@%s%s", YELLOW, username, host, WHITE);
        printf(":");
        printf("%s%s%s", CYAN, current_directory, WHITE);
        printf("> ");

        fgets(lastCommand, STRMAX, stdin);
        strcpy(commandInit, lastCommand);
        if ( trocearCadena(lastCommand, segmentedCommand) != 0 ) {
            if ( strcmp(lastCommand, "exit") == 0 ||
                    strcmp(lastCommand, "end") == 0 ||
                    strcmp(lastCommand, "quit") == 0 )
            {
                finished = true;
                clearList(historic);
                clearMemList(memList);
                free(memList);
                free(historic);
            } else {
                if ( strcmp(lastCommand, "historic") == 0
                    && historicArgumentType(segmentedCommand) == 2 ) {
                    processCommand(&processList, segmentedCommand);

                } else {
                    if( !insertItemList(historic, NULL, commandInit) ) {
                        printf("Error: Imposible to save historic");
                    }

                    processCommand(&processList, segmentedCommand);
                }
            }
        }

    } while ( !finished );

    return 0;
}

int listArgumentType(char* segmentedCommand[], bool arguments[4]) {
    int i = 1, j = 0;
    bool fin = true;

    while ( i < 5 && segmentedCommand[i] != NULL && fin ) { //Loop to determine which arguments were recived
        if ( strcmp(segmentedCommand[i], "-long") == 0 ) {
            arguments[0] = true;
            j++;
        } else if ( strcmp(segmentedCommand[i], "-dir") == 0 ) {
            arguments[1] = true;
            j++;
        } else if ( strcmp(segmentedCommand[i], "-hid") == 0 ) {
            arguments[2] = true;
            j++;
        } else if ( strcmp(segmentedCommand[i], "-rec") == 0 ) {
            arguments[3] = true;
            j++;
        } else {
            fin = false;
        }
        i++;
    }
    return j;
}

int createArgumentType(char* segmentedCommand[]) {
    if ( segmentedCommand[1] == NULL)
        return 0;

    if ( segmentedCommand[2] == NULL )
        return 1;

    if ( segmentedCommand[3] != NULL)
        return -2;

    if ( strcmp(segmentedCommand[1], "-dir") == 0 )
        return 2;

    return -1;
}

int deleteArgumentType(char* segmentedCommand[]) {
    if ( segmentedCommand[1] == NULL )
        return 0;

    if ( strcmp(segmentedCommand[1], "-rec") == 0 ) {
        if ( segmentedCommand[2] == NULL )
            return 0;
        else
            return 1;
    }
    return 2;
}

int historicArgumentType(char * segmentedCommand[]) {
    char* argument = segmentedCommand[1];

    if ( argument == NULL ) {
        return 0;

    } else {
        if( segmentedCommand[2] != NULL ) {
            return -2;

        } else if ( argument[0] == '-' ) {

            if ( argument[1] == 'c' ) {
                if ( argument[2] == '\0' )
                    return 1;

            } else if ( argument[1] == 'r'  ) {
                if( argument[2] >= '0' && argument[2] <= '9' )
                    return 2;
    
            } else if ( argument[1] >= '0' && argument[1] <= '9' )
                return 3;
        }
    }
    return -1;
}

void printPWD() {
    char cwd[STRMAX];
    if ( getcwd(cwd, sizeof(cwd)) != NULL ) {
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
    time_t timer = time(NULL);
    char buffer[26];

    strftime(buffer, 26, "%d/%m/%y", localtime(&timer));
    puts(buffer);
}

void printTime() {
    time_t timer = time(NULL);
    char buffer[26];
   
    strftime(buffer, 26, "%H:%M:%S", localtime(&timer));
    puts(buffer);
}

void printAuthors(char * segmentedCommand[]) {
    if ( segmentedCommand[1] == NULL) {
        printf("Leopoldo Estevez: leopoldo.estevez \nDavid Rodríguez: david.rbacelar \n");
        return;
    }
    if ( segmentedCommand[2] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }
    if ( strcmp(segmentedCommand[1], "-l" ) == 0 )
        printf("leopoldo.estevez\ndavid.rbacelar\n");

    else if ( strcmp(segmentedCommand[1], "-n") == 0 )
        printf("Leopoldo Estevez\nDavid Rodríguez\n");

    else
        printf("Error: %s\n", strerror(EINVAL));
}

void changeDirectory(char * segmentedCommand[]) {
    char cwd[STRMAX];

    if ( segmentedCommand[1] == NULL ) {
        if ( getcwd(cwd, sizeof(cwd)) != NULL )
            printf("Current working dir: %s\n", cwd);
        else
            printf("Error: Cannot get CWD");
        return;    
    }
    if ( segmentedCommand[2] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }
    if ( chdir(segmentedCommand[1]) != 0 )
        printf("Error: %s\n", strerror(errno));
    
}

void manageHistoric(ArrayList* processList, char * segmentedCommand[]) {
    char* argument = segmentedCommand[1];
    int historicArgument = historicArgumentType(segmentedCommand);
    int argumentNumericValue;
    char* fetchedCommand;
    char commandInit[STRMAX];

    if ( historicArgument == -2 )
        printf("Error: %s\n", strerror(E2BIG));

    else if ( historicArgument == -1 )
        printf("Error: %s\n", strerror(EINVAL));

    else if ( historicArgument == 0 ) //Print all historic
        printList(historic, -1);

    else if ( historicArgument == 1 ) { //Clear historic
        clearList(historic);
        printf("The history has been cleared\n");

    } else if ( historicArgument == 2 ) { //Repeat historic entry
        argumentNumericValue = atoi(&argument[2]);

        if ( (fetchedCommand = getItem(historic, argumentNumericValue)) != NULL ) {
            strcpy(commandInit, fetchedCommand);
            printf("Running the command (%d) > %s", argumentNumericValue, commandInit);
            trocearCadena(commandInit, segmentedCommand);
            processCommand(processList, segmentedCommand);

        } else
            printf("Error: Index out of bounds\n");

    } else { //Print a number of historic entries
        int maxCounted = atoi(&argument[1]);
        printList(historic, maxCounted);
    }
}

/*-----------FILE-SYSTEM-----------*/

void createDirFile(char* segmentedCommand[]) {
    int createArgument = createArgumentType(segmentedCommand);

    if ( createArgument == -2 )
        printf("Error: %s\n", strerror(E2BIG));

    else if ( createArgument == -1 )
        printf("Error: %s\n", strerror(EINVAL));

    else if ( createArgument == 0 ) //Short List the current directory
        printPath(".", false, true, false, 0);

    else if ( createArgument == 1 ) { //Create a file
        if ( open(segmentedCommand[1], O_CREAT | O_EXCL, 0777) == -1 )
            printf("Error: Cannot create file %s: %s\n", segmentedCommand[1], strerror(errno));
        else
            printf("Created: %s (file)\n", segmentedCommand[1]);

    } else { //Create a directory
        if ( mkdir(segmentedCommand[2], 0777) == -1 )
            printf("Error: Cannot create directory %s: %s\n", segmentedCommand[2], strerror(errno));
        else
            printf("Created: %s (dir)\n", segmentedCommand[2]);
    }
}

void deleteDirFile(char* segmentedCommand[]) {
    int deleteArgument = deleteArgumentType(segmentedCommand);
    int i;
    struct stat statbuf;

    if ( deleteArgument == 0 ) { //Short List the current directory
        printPath(".", false, true, false, 0);

    } else if ( deleteArgument == 1 ) { //Delete recursive
        i = 2;
        while ( segmentedCommand[i] != NULL ) {
            if ( lstat(segmentedCommand[i], &statbuf) == 0 ) {
                if ( LetraTF(statbuf.st_mode) != 'd' ) {
                    if ( unlink(segmentedCommand[i]) == 0 )
                        printf("%sDeleted: %s%s\n", RED, segmentedCommand[i], WHITE);
                    else
                        printf("Error: Cannot delete file %s: %s\n", segmentedCommand[i], strerror(errno));
                      
                } else
                    deleteRecursive(segmentedCommand[i]);

            } else
                printf("Error: Cannot access %s: %s\n", segmentedCommand[i], strerror(errno));

            i++;
        }

    } else { //Normal delete
        i = 1;
        while ( segmentedCommand[i] != NULL ) {
            if ( lstat(segmentedCommand[i], &statbuf) == 0 ) {
                if ( LetraTF(statbuf.st_mode) != 'd' ) {
                    if ( unlink(segmentedCommand[i]) == 0 )
                        printf("%sDeleted: %s%s\n", RED, segmentedCommand[i], WHITE);
                    else
                        printf("Error: Cannot delete file: %s\n", strerror(errno));

                } else {
                    if ( rmdir(segmentedCommand[i]) == 0 )
                        printf("%sDeleted: %s (dir)%s\n", RED, segmentedCommand[i], WHITE);
                    else
                        printf("Error: Cannot delete directory: %s\n", strerror(errno));
                }

            } else
                printf("Error: Cannot access %s: %s\n", segmentedCommand[i], strerror(errno));

            i++;
        }
    }
}

void deleteRecursive(char* path) {
    struct dirent * elem;
    DIR* directory;
    struct stat statbuf;
    char aux[STRMAX];

    if ( (directory = opendir(path)) != NULL) {
        elem = readdir(directory);
        while ( elem != NULL ) { //readdir() returns NULL when there are no more files in the directory
            strcpy(aux, path);
            if ( (strcmp(elem->d_name, ".") != 0) && (strcmp(elem->d_name, "..") != 0) ) {

                if ( aux[strlen(aux) - 1] == '/' ) //We manage the relative routes
                    strcat(aux, elem->d_name);
                else
                    strcat(strcat(aux, "/"), elem->d_name);

                if ( lstat(aux, &statbuf) == 0 ) {
                    if ( LetraTF(statbuf.st_mode) == 'd' )
                        deleteRecursive(aux); //Recursive call

                    else {
                        if ( unlink(aux) == 0 )
                            printf("%sDeleted: %s%s\n", RED, aux, WHITE);
                        else
                            printf("Error: Cannot delete file: %s\n", strerror(errno));
                    }

                } else
                    printf("Error: Cannot access %s: %s", aux, strerror(errno));
            }

        elem = readdir(directory);
        }
    }

    if ( rmdir(path) == 0 )
        printf("%sDeleted: %s (dir)%s\n", RED, path, WHITE);
    else
        printf("Error: Cannot delete directory: %s\n", strerror(errno));

    closedir(directory);
}

void listDirFile(char* segmentedCommand[]) {
    bool islong = false, isdir = false, ishid = false, isrec = false;
    bool arguments[4] = {islong, isdir, ishid, isrec};
    int ppath, numarguments, commandlength = 0, numfiles;
    struct stat statbuf;

    numarguments = listArgumentType(segmentedCommand, arguments);
    while( segmentedCommand[commandlength] != NULL ) {
        commandlength++;
    }
    numfiles = commandlength - numarguments - 1; //Number of files or directories specified by the user
    ppath = numarguments + 1; //Position where the files or directories start

    if ( arguments[1] ) {
        if ( numfiles == 0 ) {
            printf("Error: No directory specified\n");
        }
        while ( segmentedCommand[ppath] != NULL ) {
            if ( lstat(segmentedCommand[ppath], &statbuf) == 0 ) {
                if ( LetraTF(statbuf.st_mode) != 'd' ) {
                    //Print only a file
                    filePrint(segmentedCommand[ppath], segmentedCommand[ppath], arguments[0]);
                } else {
                    //Print the content of the dir
                    printPath(segmentedCommand[ppath], arguments[0], arguments[2], arguments[3], 0);
                }
            } else {
                printf("Error: Cannot access %s: %s\n", segmentedCommand[ppath], strerror(errno));
            }
            ppath++;
        }

    } else {
        if ( numfiles == 0) { //If the user doesen't specify a file or dir NEITHER the -dir argument, the path to print is the current directory
            printPath(".", arguments[0], arguments[2], arguments[3], 0);
        } else {
            while ( segmentedCommand[ppath] != NULL ) {
                filePrint(segmentedCommand[ppath], segmentedCommand[ppath], arguments[0]);
                ppath++;
            }
        }
    }
}

void printPath(char* path, bool islong, bool ishide, bool isrec, int count) {
    struct dirent * elem;
    DIR* maindirectory;
    DIR* subdirectory;
    DIR* testdir;
    char aux[STRMAX];
    struct stat statbuf;

    if ( (testdir = opendir(path)) != NULL ) {
        for ( int j = 0; j<=1; j++ ) { //We scan two times the same folder; the first time we only print the files, and the second time we only print the folders.
            maindirectory = opendir(path);
            elem = readdir(maindirectory);
            while ( elem != NULL ) { //readdir() returns NULL when there are no more files in the directory
                strcpy(aux, path);
                if ( ishide || elem->d_name[0] != '.') { //If ishide is false, the second condition is evaluated

                    if(aux[strlen(aux) - 1] == '/') { //We manage the relative routes
                        strcat(aux, elem->d_name);
                    } else {
                        strcat(strcat(aux, "/"), elem->d_name);
                    }

                    if ( lstat(aux, &statbuf) == -1 ) { //We get the info about the file or dir
                        printf("Error: Cannot access %s: %s\n", aux, strerror(errno));

                    } else {

                        if ( (strcmp(elem->d_name, ".") != 0) && (strcmp(elem->d_name, "..") != 0) &&
                            ((LetraTF(statbuf.st_mode) == 'd' && j == 1) || (LetraTF(statbuf.st_mode) != 'd' && j == 0)) ) {

                            if ( isrec ) { //If isrec is false it will only print the file OR dir; else it will try to open the path given

                                for ( int i = 0; i<count; i++ ) { //"Tree" structure
                                    if( i > 0 ) { printf("│   "); }
                                    else { printf("    "); }
                                }
                                if ( count > 0 ) { printf("├──> "); }

                                if ( LetraTF(statbuf.st_mode) == 'd' ) {
                                    if ( (subdirectory = opendir(aux)) != NULL ) {
                                        filePrint(aux, elem->d_name, islong);
                                        printPath(aux, islong, ishide, isrec, count+1); //Recursive call
                                    } else {
                                        printf("%sError: Cannot open %s: %s%s\n", RED, aux, strerror(errno), WHITE);
                                    }
                                    closedir(subdirectory);
                                } else {
                                    filePrint(aux, elem->d_name, islong);
                                }

                            } else {
                                filePrint(aux, elem->d_name, islong);
                            }
                        }
                    }

                }
                elem = readdir(maindirectory);
            }

            closedir(maindirectory);
        }

    } else {
        printf("%sError: Cannot open %s: %s%s\n", RED, path, strerror(errno), WHITE);
    }
    closedir(testdir);
}

void unitsize(float size, char unitsize[STRMAX]) {
    int i = 0;

    while ( (i < 3) && (size / 1024 > 1) ) {
        size = size / 1024;
        i++;
    }
    sprintf(unitsize, "%.2f", size);
    if ( i == 0 )
        strcat(unitsize, " B");
    else if ( i == 1 )
        strcat(unitsize, " KB");
    else if ( i == 2 )
        strcat(unitsize, " MB");
    else if ( i == 3 )
        strcat(unitsize, " GB");
}

char* getUser(uid_t uid) {
    struct passwd* uidbuf;
    
    if ( (uidbuf = getpwuid(uid)) == NULL )
        return ("??????");
    else 
        return uidbuf->pw_name;
}

char* getGroup(uid_t uid) {
    struct group* gidbuf;

    if ( (gidbuf = getgrgid(uid)) == NULL )
        return ("??????");
    else 
        return gidbuf->gr_name;
}

void filePrint(char* path, char* name, bool islong) {
    char permisos[STRMAX], linkedname[STRMAX], size[STRMAX];
    struct stat statbuf;
    char typeoffile;
    char time[50];
    char* userName;
    char* groupName;
    int i = 0;

    if ( lstat(path, &statbuf) == 0 ) {
        typeoffile = LetraTF(statbuf.st_mode);
        unitsize((float)statbuf.st_size, size);
        if ( islong ) {
            userName = getUser(statbuf.st_uid);
            groupName = getGroup(statbuf.st_gid);
            strftime(time, 50, "%b %d %H:%M", localtime(&statbuf.st_mtime));
            printf("%s%s %s%8ld %5s %5s %s %s%10s (%ld) ", PINK, time, BLUE, statbuf.st_ino,
                userName, groupName, ConvierteModo(statbuf.st_mode, permisos),
                PINK, size, statbuf.st_nlink);
            //Statment to print the files and the directories with different colors.
            if ( typeoffile == 'd' ) {
                i = strlen(name) - 1;
                if ( name[i] == '/') {
                    name[i] = '\0';
                }
                printf("%s%s/%s", GREEN, name, WHITE);
            } else
                printf("%s%s%s", CYAN, name, WHITE);

            //If the file is a link, we add the name of its pointed file.
            if ( typeoffile == 'l' ) {
                readlink(path, linkedname, STRMAX);
                printf("%s -> %s%s", CYAN, linkedname, WHITE);
            }

        } else {
            if ( typeoffile == 'd' ) {
                i = strlen(name) - 1;
                if ( name[i] == '/') {
                    name[i] = '\0';
                }
                printf("%s%s/", GREEN, name);
            } else
                printf("%s%s", CYAN, name);

            printf("%s (%s)%s", PINK, size, WHITE);
        }
        printf("\n");

    } else
        printf("%sError: Cannot access %s: %s%s\n", RED, path, strerror(errno), WHITE);
}

/*-----------MEMORY-----------*/

void memoryOperation(char* segmentedCommand[]) {
    if ( segmentedCommand[1] == NULL )
        printMemList(memList, NULL);
	else if ( strcmp(segmentedCommand[1], "-help") == 0 ) 
		memoryHelp();
    else if ( strcmp(segmentedCommand[1], "-allocate") == 0 )
        memoryAllocate(segmentedCommand);
    else if ( strcmp(segmentedCommand[1], "-dealloc") == 0 )
        memoryDeallocate(segmentedCommand);
    else if ( strcmp(segmentedCommand[1], "-deletekey") == 0 ) {
        if ( segmentedCommand[2] == NULL )
            printf("Error: Too few arguments\n");
        else if ( segmentedCommand[3] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            memoryDeleteKey(segmentedCommand[2]);
    }
    else if ( strcmp(segmentedCommand[1], "-show") == 0 )
        memoryShow(segmentedCommand);
    else if ( strcmp(segmentedCommand[1], "-show-vars") == 0 )
        memoryShowVarFun(segmentedCommand);
    else if ( strcmp(segmentedCommand[1], "-show-funcs") == 0 )
        memoryShowVarFun(segmentedCommand);
    else if ( strcmp(segmentedCommand[1], "-dopmap") == 0 )
        memoryPmap();
    else
        printf("Error: %s\n", strerror(EINVAL));
}

void memoryAllocate(char* segmentedCommand[]) {
    if ( segmentedCommand[2] == NULL )
        printMemList(memList, NULL);
    else if ( strcmp(segmentedCommand[2], "-malloc") == 0 )
        memoryAllocateMalloc(segmentedCommand);
    else if ( strcmp(segmentedCommand[2], "-mmap") == 0 )
        memoryAllocateMmap(segmentedCommand);
    else if ( strcmp(segmentedCommand[2], "-createshared") == 0 )
        memoryAllocateCreateShared(segmentedCommand);
    else if ( strcmp(segmentedCommand[2], "-shared") == 0 )
        memoryAllocateShared(segmentedCommand);
    else
        printf("Error: %s\n", strerror(EINVAL));
}

void memoryDeallocate(char* segmentedCommand[]) {
    if ( segmentedCommand[2] == NULL )
        printMemList(memList, NULL);
    else if ( strcmp(segmentedCommand[2], "-malloc") == 0 )
        memoryDeallocMalloc(segmentedCommand);
    else if ( strcmp(segmentedCommand[2], "-mmap") == 0 )
        memoryDeallocMmap(segmentedCommand);
    else if ( strcmp(segmentedCommand[2], "-shared") == 0 )
        memoryDeallocShared(segmentedCommand);
    else
        memoryDeallocAddr(segmentedCommand);
}

void memoryShow(char* segmentedCommand[]) {
    int local1 = 0;
    char local2 = 'a';
    float local3 = 1.2;

    if ( segmentedCommand[2] == NULL ) {
        printf("Local variables:      %p,   %p,   %p\n", &local1, &local2, &local3);
        printf("Global variables:     %p,   %p,   %p\n", &global1, &global2, &global3);
        printf("Program functions:    %p,   %p,   %p\n", &processCommand, &memoryOperation, &memoryShowVarFun);
    } else if ( segmentedCommand[3] != NULL )
        printf("Error: %s\n", strerror(E2BIG));
    else if ( strcmp(segmentedCommand[2], "-malloc") == 0 )
        printMemList(memList, "malloc");
    else if ( strcmp(segmentedCommand[2], "-mmap") == 0 )
        printMemList(memList, "mmap");
    else if ( strcmp(segmentedCommand[2], "-shared") == 0 )
        printMemList(memList, "shared");
    else if ( strcmp(segmentedCommand[2], "-all") == 0 )
        printMemList(memList, NULL);
    else
        printf("Error: %s\n", strerror(EINVAL));
    
}

void memoryAllocateMalloc(char* segmentedCommand[]) {
    void* p;
    struct memNode node;
    size_t size;

    if ( segmentedCommand[3] == NULL ) {
        printMemList(memList, "malloc");
        return;
    } 
    if ( segmentedCommand[4] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }

    if ( (size_t) (size = atol(segmentedCommand[3])) == 0 ) {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    if ( (p = malloc(size)) == NULL) {
        printf("Error: Cannot allocate %s bytes of memory\n", segmentedCommand[3]);
        return;
    }
    printf("Allocated %s bytes at %p\n", segmentedCommand[3], p);
    node.fd = 0;
    strcpy(node.filename, "");
    node.time = time(NULL);
    node.key = 0;
    node.next = NULL;
    node.pointer = p;
    strcpy(node.type, "malloc");
    node.size = size;

    insertItemMemList(memList, NULL, node); 
}

void memoryAllocateMmap(char* segmentedCommand[]) {
    char* perm;
    void *p;
    int fd, protection = 0, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat statbuf;
    struct memNode node;

    if ( segmentedCommand[3] == NULL ) {
        printMemList(memList, "mmap");
        return;
    }

    if ( (perm = segmentedCommand[4]) != NULL && strlen(perm) < 4 ) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }

    if (protection & PROT_WRITE) modo = O_RDWR;

    if ( (fd = open(segmentedCommand[3], modo)) == -1 ) {
        printf("Error: Cannot open %s: %s\n", segmentedCommand[3], strerror(errno));
        return;
    }
    if ( lstat(segmentedCommand[3], &statbuf) == -1 ) {
        printf("Error: Cannot access %s: %s\n", segmentedCommand[3], strerror(errno));
        return;
    }
    if ( (p = mmap(NULL, statbuf.st_size, protection, map, fd, 0)) == MAP_FAILED ) {
        printf("Error: Cannot map file %s: %s\n", segmentedCommand[3], strerror(errno));
        return;
    }
    printf("File %s mapped at %p\n", segmentedCommand[3], p);
    node.fd = fd;
    strcpy(node.filename, segmentedCommand[3]);
    node.time = time(NULL);
    node.key = 0;
    node.size = statbuf.st_size;
    node.next = NULL;
    node.pointer = p;
    strcpy(node.type, "mmap");

    insertItemMemList(memList, NULL, node);    
}

void memoryAllocateCreateShared(char* segmentedCommand[]) {
    key_t k;
    size_t tam;
    void *p;
    int id, flags = 0777;
    struct shmid_ds s;
    struct memNode node;
    
    if ( segmentedCommand[3] == NULL ) {
        printMemList(memList,"shared");
        return;
    }
    if ( segmentedCommand[4] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    if ( segmentedCommand[5] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }
    if ( (k = (key_t) atoi(segmentedCommand[3])) == 0 || k == IPC_PRIVATE ) {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    if ( (tam = (size_t) atoll(segmentedCommand[4])) == 0 && strcmp(segmentedCommand[4], "0") != 0 ) {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    if ( tam ) 
        flags = flags | IPC_CREAT | IPC_EXCL;

    if ( (id = shmget(k, tam, flags) ) == -1 ) {
        printf("Error: Cannot create shared memory (Key: %d, Size: %ld): %s\n", k, tam, strerror(errno));
        return;
    }
    if ( (p = shmat(id, NULL, 0)) == (void*) -1 ) {
        printf("Error: Cannot asign shared memory (Key: %d, Size: %ld): %s\n", k, tam, strerror(errno));
        if ( tam ) shmctl(id, IPC_RMID, NULL);
        return;
    }

    shmctl(id, IPC_STAT, &s);
    printf("Shared memory (Key: %d, Size: %ld) assigned at %p\n", k, s.shm_segsz, p);
    node.fd = 0;
    strcpy(node.filename, "");
    node.time = time(NULL);
    node.key = k;
    node.size = s.shm_segsz;
    node.next = NULL;
    node.pointer = p;
    strcpy(node.type, "shared");

    insertItemMemList(memList, NULL, node);
}

void memoryAllocateShared(char* segmentedCommand[]) {
    if ( segmentedCommand[3] == NULL )
        printMemList(memList, "shared");

    else if ( segmentedCommand[4] != NULL )
        printf("Error: %s\n", strerror(E2BIG));

    else {
        segmentedCommand[4] = "0";
        segmentedCommand[5] = NULL;
        memoryAllocateCreateShared(segmentedCommand);
    }
}

void memoryDeallocMalloc(char* segmentedCommand[]) {
    size_t size;
    PosM p;

    if ( segmentedCommand[3] == NULL )
        printMemList(memList, "malloc");

    else if ( segmentedCommand[4] != NULL )
        printf("Error: %s\n", strerror(E2BIG));

    else {
        if ( (size = (size_t) atoll(segmentedCommand[3])) == 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
        if ( (p = getMallocItem(memList, size)) == NULL ) {
            printf("Error: Cannot find memory allocated with malloc of size %ld\n", size);
            return;
        }
        printf("%sDeallocated %ld bytes at %p%s\n", RED, size, p->pointer, WHITE);
        free(p->pointer);
        deleteItemMemList(memList, p);
    }
}

void memoryDeallocMmap(char* segmentedCommand[]) {
    PosM p;

    if ( segmentedCommand[3] == NULL )
        printMemList(memList, "mmap");

    else if ( segmentedCommand[4] != NULL )
        printf("Error: %s\n", strerror(E2BIG));

    else {
        if ( (p = getMmapItem(memList, segmentedCommand[3])) == NULL ) {
            printf("Error: Cannot find mapped file %s\n", segmentedCommand[3]);
            return;
        }
        close(p->fd);
        if ( munmap(p->pointer, p->size) == -1 ) {
            printf("Error: Cannot unmap file %s: %s\n", segmentedCommand[3], strerror(errno));
            return;
        }
        printf("%sUnmaped file %s at %p%s\n", RED, segmentedCommand[3], p->pointer, WHITE);
        deleteItemMemList(memList, p);
    }
}

void memoryDeallocShared(char* segmentedCommand[]) {
    PosM p;
    key_t key;

    if ( segmentedCommand[3] == NULL )
        printMemList(memList, "shared");

    else if ( segmentedCommand[4] != NULL )
        printf("Error: %s\n", strerror(E2BIG));

    else {
        if ( (key = (key_t) atoi(segmentedCommand[3])) == 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
        if ( (p = getSharedItem(memList, key)) == NULL ) {
            printf("Error: Cannot find shared memory (Key: %d)\n", key);
            return;
        }
        if ( shmdt(p->pointer) == -1 ) {
            printf("Error: Cannot deatach shared memory (Key: %d): %s\n", key, strerror(errno));
            return;
        }
        printf("%sDeatached shared memory (Key: %d) at %p%s\n", RED, key, p->pointer, WHITE);
        deleteItemMemList(memList, p);
    }

}

void memoryDeallocAddr(char* segmentedCommand[]) {
    void* dir;
    PosM p;
    char* endptr;

    if ( segmentedCommand[3] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }
    dir = (void*)strtol(segmentedCommand[2], &endptr, 16);
    if ( *endptr != '\0') {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    if ( (p = searchAddr(memList, dir)) == NULL ) {
        printf("Error: Cannot find adrress %p\n", dir);
        return;
    }
    if ( strcmp(p->type, "malloc") == 0 ) {
        printf("%sDeallocated %ld bytes at %p%s\n", RED, p->size, p->pointer, WHITE);
        free(p->pointer);

    } else if ( strcmp(p->type, "mmap") == 0 ) {
        close(p->fd);
        if ( munmap(p->pointer, p->size) == -1 ) {
            printf("Error: Cannot unmap file %s: %s\n", p->filename, strerror(errno));
            return;
        }
        printf("%sUnmaped file %s at %p%s\n", RED, p->filename, p->pointer, WHITE);

    } else {
        if ( shmdt(p->pointer) == -1 ) {
            printf("Error: Cannot deatach shared memory (Key: %d): %s\n", p->key, strerror(errno));
            return;
        }
        printf("%sDeatached shared memory (Key: %d) at %p%s\n", RED, p->key, p->pointer, WHITE);
    }
    deleteItemMemList(memList, p);
    
}

void memoryDeleteKey(char* key) {
    key_t clave;
    int id;

    if ( (clave = (key_t) strtoul(key, NULL, 10)) == IPC_PRIVATE ) {
        printf ("Error: Cannot delete key %s\n", key);
        return;
    }
    if ( (id = shmget(clave, 0, 0666)) == -1 ) {
        printf("Error: Cannot get shared memory (Key: %d): %s\n", clave, strerror(errno));
        return;
    }
    if ( shmctl( id, IPC_RMID, NULL) == -1 ) {
        printf("Error: Cannot remove shared memory (Key: %d): %s\n", clave, strerror(errno));
        return;
    }
    printf("%sRemoved: Key %d%s\n", RED, clave, WHITE);
}

void memoryPmap() {
    pid_t pid;
    char elpid[32];
    char* argv[3] = {"pmap", elpid, NULL};
    sprintf(elpid, "%d", (int) getpid());

    if ( (pid = fork()) == -1 )
        printf("Error: Cannot create the process\n");

    else if ( pid == 0 ) {
        if ( execvp(argv[0], argv) == -1 )
            printf("Error: Cannot execute pmap\n");
        else 
            printf("Process map created\n");
    }
    waitpid(pid, NULL, 0);
}

void memoryShowVarFun(char* segmentedCommand[]) {
    int local1 = 0;
    char local2 = 'a';
    float local3 = 1.2;

    if ( segmentedCommand[2] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    } 
    if (strcmp(segmentedCommand[1], "-show-vars") == 0 ) {
        printf("Local variables:     %p,   %p,   %p\n", &local1, &local2, &local3);
        printf("Global variables:    %p,   %p,   %p\n", &global1, &global2, &global3);
    } else {
        printf("Program functions:    %p,   %p,   %p\n", &processCommand, &memoryOperation, &memoryShowVarFun);
        printf("Library functions:    %p,   %p,   %p\n", &atoi, &printf, &strerror);
    }
}

void memDump(char* segmentedCommand[]) {
    int count = 0;
    char* ptr;
    char* endptr;

    if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    if ( segmentedCommand[2] == NULL )
        count = 25;

    else if ( segmentedCommand[3] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;

    } else if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }

    ptr = (char*)strtol(segmentedCommand[1], &endptr, 16);
    if ( *endptr != '\0') {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    for ( int i=0; i<count; i=i+25 ) {
        for ( int j=i; j<count && j<(i+25); j++ ) {
            if ( *(ptr+j) < ' ' )
                printf("   ");
            else
                printf("%2c ", *(ptr+j));
        }
        printf("\n");
        for ( int j=i; j<count && j<(i+25); j++ ) {
            printf("%02X ", *(ptr+j));
        }
        printf("\n\n");
    }
}

void memFill(char* segmentedCommand[]) {
    int count;
    int byte;
    char* ptr;
    char* endptr;

    if (segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
    } else if ( segmentedCommand[2] == NULL ) {
        count = 128;
        byte  = 65;
    } else if ( segmentedCommand[3] == NULL ) {
        if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
        byte = 65;
    } else if ( segmentedCommand[4] != NULL) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
        
    } else {
        if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
        if ( (byte = (int) strtol(segmentedCommand[3], NULL, 16)) == 0 && strcmp(segmentedCommand[3], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
    }

    ptr = (char*)strtol(segmentedCommand[1], &endptr, 16);
    if ( *endptr != '\0') {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    for ( int i=0; i<count; i++ ) {
        *(ptr+i) = (char) byte;
    }
    printf("%sMem fill: Success%s\n", GREEN, WHITE);

}

void doRecursiveAux(int n) {
    char automaticarr[4096];
    static char staticarr[4096];

    printf("Parameter %02d at:    %p\n", n, &n);
    printf("Static Array at:    %p \n", staticarr);
    printf("Automatic Array at: %p\n\n", automaticarr);
    printf("--------------\n\n");
    n--;
    if ( n > 0 )
        doRecursiveAux(n); 
}

void doRecursive(char* segmentedCommand[]) {
    int n;

    if ( segmentedCommand[1] == NULL )
        printf("Error: Too few arguments\n");

    else if ( segmentedCommand[2] != NULL )
        printf("Error: %s\n", strerror(E2BIG));

    else {
        if ( (n = atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0 )
            printf("Error: %s\n", strerror(EINVAL));
        else
            doRecursiveAux(n);
    }
}

void readFile(char* segmentedCommand[]) {
    ssize_t tam, n;
    int df;
    char* endptr;
    void* p;
    struct stat s;
    char* fich;

    if ( segmentedCommand[1] == NULL || segmentedCommand[2] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    if ( segmentedCommand[3] == NULL ) {
        n = LEERCOMPLETO;

    } else if ( segmentedCommand[4] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    } else {
        if ( (n = atoi(segmentedCommand[3])) == 0 && strcmp(segmentedCommand[3], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
    }
    
    fich = segmentedCommand[1];
    tam = n;
    p = (void*)strtol(segmentedCommand[2], &endptr, 16);
    if ( *endptr != '\0') {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }
    if ( lstat(fich, &s) == -1 || (df = open(fich, O_RDONLY)) == -1 ) {
        printf("Error: Cannot read file: %s\n", strerror(errno));
        return;
    }
    if ( n == LEERCOMPLETO )
        tam = (ssize_t)s.st_size;
    if ( read(df, p, tam) == -1 ) {
        printf("Error: Cannot read file: %s\n", strerror(errno));
    } else {
        printf("%sRead file: Success%s\n", GREEN, WHITE);
    }
    close(df);
}

void writeFile(char* segmentedCommand[]) {
    ssize_t tam, n;
    int df, i = 0;
    char* endptr;
    void* p;
    struct stat s;
    char* fich;
    int flags = O_CREAT | O_WRONLY;
    int mode = 00777;

    if ( strcmp(segmentedCommand[1], "-o") == 0 )
        i = 1;	
    else if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    else
        flags = flags | O_EXCL;
        
    if ( segmentedCommand[1+i] == NULL || segmentedCommand[2+i] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    if ( segmentedCommand[3+i] == NULL ) {
        n = LEERCOMPLETO;

    } else if ( segmentedCommand[4+i] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }

    if ( n != LEERCOMPLETO ) {
        if ( (n = atoi(segmentedCommand[3+i])) == 0 && strcmp(segmentedCommand[3+i], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
    }
    
    fich = segmentedCommand[1+i];
    tam = n;
    p = (void*)strtol(segmentedCommand[2+i], &endptr, 16);
    if ( *endptr != '\0') {
        printf("Error: %s\n", strerror(EINVAL));
        return;
    }

    if ( (df = open(fich, flags, mode)) == -1 || lstat(fich, &s) == -1 ) {
        printf("Cannot open/create file: %s\n", strerror(errno));
        return;
    }
    if ( n == LEERCOMPLETO )
        tam = (ssize_t)s.st_size;
    if ( write(df, p, tam) == -1 ) {
        printf("Error: Cannot write file: %s\n", strerror(errno));
    } else {
        printf("%sWrite file: Success%s\n", GREEN, WHITE);
    }
    close(df);
}

/*-----------PROCESSES-----------*/

void getPriority(char* segmentedCommand[]) {
    pid_t pid;
    int nice;

    errno = 0;
    if ( segmentedCommand[1] == NULL ) {
        nice = getpriority(PRIO_PROCESS, 0);

    } else if ( segmentedCommand[2] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
        
    } else {
        if ( (pid = (pid_t) atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return; 
        }
        nice = getpriority(PRIO_PROCESS, pid);
    }
    if ( errno != 0 )
            printf("Error: Cannot get process priority: %s\n", strerror(errno));
        else
            printf("Process priority: %d\n", nice);
}

void setPriority(char* segmentedCommand[]) {
    pid_t pid = 0;
    int nice;

    if ( segmentedCommand[1] == NULL ) {
        nice = getpriority(PRIO_PROCESS, 0);
        if ( errno != 0 )
            printf("Error: Cannot get process priority: %s\n", strerror(errno));
        else
            printf("%d", nice);

    } else if ( segmentedCommand[2] == NULL ) {
        if ( (nice = atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }

    } else if ( segmentedCommand[3] == NULL ) {
        if ( ((pid = (pid_t) atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0) || 
            ((nice = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0) ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }

    } else {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }

    if ( setpriority(PRIO_PROCESS, pid, nice) == -1 )
        printf("Error: Cannot change process priority: %s\n", strerror(errno));

    else {
        if ( pid == 0 )
            printf("Current process priority changed to %d\n", nice);  
        else
            printf("Pid %d priority changed to %d \n", pid, nice);
    }
}

uid_t userUid(char* name) {
    struct passwd *p;
    if ( (p = getpwnam(name)) == NULL )
        return (uid_t) -1;
    return p->pw_uid;
}

void getUid() {
    uid_t real = getuid(), efective = geteuid();
    printf("Real UID: %d (%s)\n", real, getUser(real));
    printf("Efective UID: %d (%s)\n", efective, getUser(efective));
}

int setUid(char* segmentedCommand[]) {
    uid_t uid;

    if ( segmentedCommand[1] == NULL ) {
        getUid();

    } else if ( strcmp(segmentedCommand[1], "-l") == 0 ) {
        if ( segmentedCommand[2] == NULL ) {
            printf("Error: Too few arguments\n");
            return -1;
        }
        if ( segmentedCommand[3] != NULL ) {
            printf("Error: %s\n", strerror(E2BIG));
            return -1;
        }
        if ( (uid = userUid(segmentedCommand[2])) == (uid_t) -1 ) {
            printf("Error: Cannot get UID: %s\n", strerror(errno));
            return -1;
        }
        
    } else {
        if ( segmentedCommand[2] != NULL ) {
            printf("Error: %s\n", strerror(E2BIG));
            return -1;
        }
        if ( (uid = atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return -1;
        }
    }

    if ( setuid(uid) == -1 ) {
        printf("Error: Cannot chenge UID: %s\n", strerror(errno));
        return -1;
    } else {
        printf("%sSet UID: Success%s\n", GREEN, WHITE);
        return 0;
    }
        
}

void forkProcess() {
    pid_t pid;

    if ( (pid = fork()) == -1 ) {
        printf("Error: Cannot create child process\n");
        
    } else if ( pid != 0 ) {
        printf("Executing child process (pid %d)\n", pid);
        waitpid(pid, NULL, 0);
    }
}

bool managePriority(char* segmentedCommand[], char* priority) {
    int i;

    for ( i = 2; segmentedCommand[i] != NULL && *(segmentedCommand[i]) != '@'; i++ );
    if ( segmentedCommand[i] == NULL )
        return false;

    else {
           strcpy(priority, segmentedCommand[i]+1);
        segmentedCommand[i] = NULL;
        return true;
    }
}

void cleanCommand(char* segmentedCommand[], char* cleanedCommand[]) {
    int p = 0;

    while ( p < 21 && segmentedCommand[p+1] != NULL ) { 
        cleanedCommand[p] = segmentedCommand[p+1];
        p++;
    }
    while ( p < 21 ) { 
        cleanedCommand[p] = NULL; 
        p++; 
    }
}

void executeCommand(char* segmentedCommand[]) {
    char priority[STRMAX];
    char* aux[3];
    char* cleanedCommand[20];

    if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    
    if ( managePriority(segmentedCommand, priority) ) {
        aux[0] = "setpriority";
        aux[1] = priority;
        aux[2] = NULL;
        setPriority(aux);
    }
    cleanCommand(segmentedCommand, cleanedCommand);
    if ( execvp(segmentedCommand[1], cleanedCommand) == -1 )
        printf("Error: %s\n", strerror(errno));
    exit(255);
}

void executeForeground(char* segmentedCommand[]) {
    char priority[STRMAX];
    char* aux[3];
    pid_t pid;
    char* cleanedCommand[20];

    if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }

    if ( (pid = fork()) == -1 ) {
        printf("Error: Cannot create child process\n");
        
    } else if ( pid == 0 ) {
        if ( managePriority(segmentedCommand, priority) ) {
            aux[0] = "setpriority";
            aux[1] = priority;
            aux[2] = NULL;
            setPriority(aux);
        }
        cleanCommand(segmentedCommand, cleanedCommand);
        if ( execvp(segmentedCommand[1], cleanedCommand) == -1 )
            printf("Error: %s\n", strerror(errno));
		exit(255);

    } else {
        printf("Executing child in foreground...\n");
        waitpid(pid, NULL, 0);
    }
}

void executeBackground(char* segmentedCommand[], ArrayList* processList) {
    char priority[STRMAX];
    char* aux[3];
    pid_t pid;
    arrayItem item;
    char* cleanedCommand[20];

    if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }

    if ( (pid = fork()) == -1 ) {
        printf("Error: Cannot create child process\n");

    } else if ( pid == 0 ) {
        if ( managePriority(segmentedCommand, priority) ) {
            aux[0] = "setpriority";
            aux[1] = priority;
            aux[2] = NULL;
            setPriority(aux);
        }
        cleanCommand(segmentedCommand, cleanedCommand);
        if ( execvp(segmentedCommand[1], cleanedCommand) == -1 )
            printf("Error: %s\n", strerror(errno));
        exit(255);

    } else {
        printf("Executing child in background...\n");
        item.pid = pid;
        item.time = time(NULL);
        item.priority = 0;
        strcpy(item.signal, "n/a");
        strcpy(item.state, "RUNNING");
        strcpy(item.command, "");
        for ( int i = 1; segmentedCommand[i] != NULL && *segmentedCommand[i] != '@'; i++ ) {
            strcat(item.command, segmentedCommand[i]);
            strcat(item.command, " ");
        }
        insertItemArrayList(item, LNULL, processList);
        sleep(1);
    }
}

void listProcs(ArrayList* processList) {
    printArrayList(processList, -1);
}

void procInfo(char* segmentedCommand[], ArrayList* processList) {
    pid_t pid;
    arrayPos pos;
	arrayItem item;
    int signal;

    if ( segmentedCommand[1] == NULL ) {
        printArrayList(processList, -1);
        return;
    } 

    if ( strcmp(segmentedCommand[1], "-fg") == 0 ) {
        if ( segmentedCommand[2] == NULL ) {
            printf("Error: Too few arguments");
            return;
        }
        if ( segmentedCommand[3] != NULL ) {
            printf("Error: %s\n", strerror(E2BIG));
            return;
        }
        if ( (pid = (pid_t) atoi(segmentedCommand[2])) == (pid_t) 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
    } else {
        if ( segmentedCommand[2] != NULL ) {
            printf("Error: %s\n", strerror(E2BIG));
            return;
        }
        if ( (pid = (pid_t) atoi(segmentedCommand[1])) == (pid_t) 0 ) {
            printf("Error: %s\n", strerror(EINVAL));
            return;
        }
    }

    if ( (pos = findItemArrayList(pid, *processList)) == LNULL ) {
        printf("Error: Cannot find background process with pid %d", pid);
        return;
    }

    if ( strcmp(segmentedCommand[1], "-fg") != 0 )
        printArrayList(processList, pid);

    else {
		item = getItemArrayList(pos, *processList);
		if ( strcmp(item.state, "RUNNING") != 0 ) {
			printf("Error: Cannot bring process %d to foreground\n", pid);
			return;
		}
        printf("Process with pid %d brought to foreground\n", pid);
        waitpid(pid, &signal, 0);
        if ( WIFEXITED(signal) ) {
            printf("Terminated Normally: %d\n", WEXITSTATUS(signal));
        } else if ( WIFSIGNALED(signal) ) {
            printf("Terminated by Signal: %s\n", sig2Str(WTERMSIG(signal)));
        } else if ( WIFSTOPPED(signal) ) {
            printf("Stopped\n");
        }
        deleteAtPositionArrayList(pos, processList);
    }
}  

void deleteProcs(char* segmentedCommand[], ArrayList* processList) {
    if ( segmentedCommand[1] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    if ( segmentedCommand[2] != NULL ) {
        printf("Error: %s\n", strerror(E2BIG));
        return;
    }
    if ( strcmp(segmentedCommand[1], "-term") == 0 )
        deleteItemsArrayList("TERMINATED NORMALLY", processList);
    else if ( strcmp(segmentedCommand[1], "-sig") == 0 )
        deleteItemsArrayList("TERMINATED BY SIGNAL", processList);
    else {
        printf("%s\n", strerror(EINVAL));
        return;
    }
    printf("%sDelete Processes: Success%s\n", GREEN, WHITE);
}

void runAs(char* segmentedCommand[], ArrayList* processList) {
    int tam = 0;
    pid_t pid;
    char priority[STRMAX];
    int i;
    char* setPrioVector[3];
    char* setUidVector[4];
	char* cleanedCommand[20];
    bool isBackground = false;
    arrayItem item;

    if ( segmentedCommand[1] == NULL || segmentedCommand[2] == NULL ) {
        printf("Error: Too few arguments\n");
        return;
    }
    while ( segmentedCommand[tam] != NULL ) { tam++; }

    if ( strcmp(segmentedCommand[tam-1], "&") == 0 ) {
        isBackground = true;
        segmentedCommand[tam-1] = NULL;
    }

    if ( (pid = fork()) == -1 ) {
        printf("Error: Cannot create child process\n");
        
    } else if ( pid == 0 ) {
        setUidVector[0] = "setuid";
        setUidVector[1] = "-l";
        setUidVector[2] = segmentedCommand[1];
        setUidVector[3] = NULL;
        if ( setUid(setUidVector) == -1 )
            exit(255);
        
        if ( managePriority(segmentedCommand, priority) ) {
            setPrioVector[0] = "setpriority";
            setPrioVector[1] = priority;
            setPrioVector[2] = NULL;
            setPriority(setPrioVector);
        }
		cleanCommand(segmentedCommand+1, cleanedCommand);
        if ( execvp(segmentedCommand[2], cleanedCommand) == -1 )
            printf("Error: %s\n", strerror(errno));
        exit(255);

    } else {
        if ( isBackground ) {
            printf("Executing child in background...\n");
            item.pid = pid;
            item.time = time(NULL);
            item.priority = 0;
            strcpy(item.signal, "n/a");
            strcpy(item.state, "RUNNING");
            strcpy(item.command, "");
            for ( i = 2; segmentedCommand[i] != NULL && *segmentedCommand[i] != '@'; i++ ) {
                strcat(item.command, segmentedCommand[i]);
                strcat(item.command, " ");
            }
            insertItemArrayList(item, LNULL, processList);
            sleep(1);

        } else {
            printf("Executing child in foreground...\n");
            waitpid(pid, NULL, 0);
        } 
    }
}

void executeAs(char* segmentedCommand[], ArrayList* processList) {
    char* setUidVector[4];

    if ( segmentedCommand[1] == NULL || segmentedCommand[2] == NULL ) {
        printf("Error: Too few argumnets\n");
        return;
    }
    setUidVector[0] = "setuid";
    setUidVector[1] = "-l";
    setUidVector[2] = segmentedCommand[1];
    setUidVector[3] = NULL;
    if ( setUid(setUidVector) == -1 )
        return;
    executeCommand(segmentedCommand+1);
}

void executeProgram(char* segmentedCommand[], ArrayList* processList) {
    int tam = 0;
    char* auxCommand[STRMAX];

    auxCommand[0] = "execute";
    while ( segmentedCommand[tam] != NULL ) { 
        auxCommand[tam+1] = segmentedCommand[tam];
        tam++; 
    }
    auxCommand[tam+1] = NULL;
    
    if ( strcmp(segmentedCommand[tam-1], "&") == 0 ) {
        auxCommand[tam] = NULL;
        executeBackground(auxCommand, processList);
    } else {
        executeForeground(auxCommand);
    }
}

void processCommand(ArrayList* processList, char * segmentedCommand[]) {
    if ( strcmp(segmentedCommand[0], "pwd") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            printPWD();

    } else if ( strcmp(segmentedCommand[0], "getpid") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            printPID();

    } else if ( strcmp(segmentedCommand[0], "getppid") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            printPPID();

    } else if ( strcmp(segmentedCommand[0], "date") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            printDate();

    } else if ( strcmp(segmentedCommand[0], "time") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            printTime();

    } else if ( strcmp(segmentedCommand[0], "authors") == 0 )
        printAuthors(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "chdir") == 0 )
        changeDirectory(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "historic") == 0 )
        manageHistoric(processList, segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "create") == 0 )
        createDirFile(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "delete") == 0 )
        deleteDirFile(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "list") == 0 )
        listDirFile(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "memory") == 0 )
        memoryOperation(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "memdump") == 0 )
        memDump(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "memfill") == 0 )
        memFill(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "readfile") == 0 )
        readFile(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "writefile") == 0 )
        writeFile(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "recurse") == 0 )
        doRecursive(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "getpriority") == 0 )
        getPriority(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "setpriority") == 0 )
        setPriority(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "getuid") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            getUid();

    } else if ( strcmp(segmentedCommand[0], "setuid") == 0 )
        setUid(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "fork") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            forkProcess();

    } else if ( strcmp(segmentedCommand[0], "execute") == 0 )
        executeCommand(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "foreground") == 0 )
        executeForeground(segmentedCommand);

    else if ( strcmp(segmentedCommand[0], "background") == 0 )
        executeBackground(segmentedCommand, processList);

    else if ( strcmp(segmentedCommand[0], "listprocs") == 0 ) {
        if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            listProcs(processList);

    } else if ( strcmp(segmentedCommand[0], "proc") == 0 )
        procInfo(segmentedCommand, processList);

    else if ( strcmp(segmentedCommand[0], "deleteprocs") == 0 )
        deleteProcs(segmentedCommand, processList);

    else if ( strcmp(segmentedCommand[0], "run-as") == 0 )
        runAs(segmentedCommand, processList);

    else if ( strcmp(segmentedCommand[0], "execute-as") == 0 )
        executeAs(segmentedCommand, processList);

	else if ( strcmp(segmentedCommand[0], "help") == 0 ) {
		if ( segmentedCommand[1] != NULL )
            printf("Error: %s\n", strerror(E2BIG));
        else
            shellHelp();

    } else
        executeProgram(segmentedCommand, processList);
}

int trocearCadena(char * cadena, char * trozos[]) {
    int i=1;

    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;

    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL) {
        i++;
    }
    return i;
}

char LetraTF(mode_t m) {
    switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /*fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
    }
}

char* ConvierteModo (mode_t m, char* permisos) {
    strcpy (permisos,"---------- ");
    permisos[0]=LetraTF(m);

    /*propietario*/
    if (m&S_IRUSR) permisos[1]='r';
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';
    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';
    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';
    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';

    return permisos;
}

void memoryHelp() {
	printf("\n\033[3m-------------------------\n    MEMORY OPERATIONS\n-------------------------\033[0m\n\n");
	printf(" memory -allocate [OPTION] [ARGS]...\t    Allocates memory in different ways depending on the option\n");
	printf("    -malloc <size>\t\t\t      Allocates <size> bytes of memory\n");
	printf("    -shared <key>\t\t\t      Gets shared memory of <key> and maps it in the proccess address space\n");
	printf("    -createshared <key> <size>\t\t      Gets and creates shared memory of <key>, <size> and maps \n");
	printf("\t\t\t\t\t         it in the proccess address space.\n");
	printf("    -mmap <file> <perm>\t\t\t      Maps in memory the <file> (all of its length starting at offset 0);\n");
	printf("\t\t\t\t\t         <perm> represents the mapping permissions (rwx format,\n");
	printf("\t\t\t\t\t         without spaces)\n");
	printf("\n memory -dealloc [OPTION] [ARGS]...\t    Deallocates memory in different ways depending on the option\n");
	printf("    -malloc <size>\t\t\t      Deallocates the first block of <size> bytes allocated with malloc\n");
	printf("    -mmap <file>\t\t\t      Unmaps and closes the <file>\n");
	printf("    -shared <key>\t\t\t      Detaches the shared memory block with <key> from the process\n");
	printf("\t\t\t\t\t         address space\n");
	printf("    -deletekey <key>\t\t\t      Removes the shared memory region of <key>.\n");
	printf("    <addr>\t\t\t\t      Searchs for a block with <addr> and deallocates it\n");

	printf("\n memory -show [OPTION]\t\t\t    Show information about the addresses inside the process memory space\n");
	printf("    -malloc\t\t\t\t      Shows the list of addresses allocated with the memory -allocate -malloc\n");
	printf("    -shared\t\t\t\t      Shows the list of addresses allocated with the memory -allocate\n");
	printf("\t\t\t\t\t         -createshared and memory -allocate -shared commands\n");
	printf("    -mmap\t\t\t\t      Shows the list of addresses allocated with the memory -allocate -mmap\n");
	printf("    -all\t\t\t\t      Shows the list of addresses allocated with any command\n");

	printf("\n memory -show-vars\t\t\t    Prints the memory addresses of three extern (global) variables and \n");
	printf("\t\t\t\t\t       three automatic (local) variables.\n");
	printf(" memory -show-funcs\t\t\t    Prints the memory addresses of three program functions and\n");
	printf("\t\t\t\t\t       three C library functions used in the program \n");
	printf(" memory -dopmap\t\t\t\t    Shows the output of the pmap command of the shell process\n");

	printf("\n memdump <addr> [cont]\t\t\t    Shows the contents of [cont] bytes starting at memory <addr>;\n");
	printf("\t\t\t\t\t       if [cont] is not specified, it shows 25 bytes. \n");
	printf(" memfill <addr> [cont] [byte]\t\t    Fills [cont] bytes of memory starting at <addr> with the value [byte];\n");
	printf("\t\t\t\t\t       if [byte] is not specified, the value of 0x42 is assumed, and\n");
	printf("\t\t\t\t\t       if [cont] is not specified, we’ll use a default value of 128\n");

	printf("\n recurse <times>\t\t\t    Show memory information about two declared arrays (static and automatic)\n");

	printf("\n readfile <file> <addr> [cont]\t\t    Reads [cont] bytes from <file> into memory <addr>; if [cont]\n");
	printf("\t\t\t\t\t       is not specified all of <file> is read onto memory.\n");

	printf("\n writefile [-o] <file> <addr> [cont]\t    Writes [cont] bytes from memory <addr> into <file>; if <file>\n");
	printf("\t\t\t\t\t       does not exist it gets created; if it already exists it is not\n");
	printf("\t\t\t\t\t       overwritten unless [-o] is specified; if [cont] is not specified,\n");
	printf("\t\t\t\t\t       it is assumed to be the <file> size\n");
}

void processHelp() {
	printf("\n\033[3m--------------------------\n    PROCESS OPERATIONS\n--------------------------\033[0m\n\n");
	printf(" getpriority [PID]\t\t\t      Shows the priority of process pid. If pid is no given specified, priority\n");
	printf("\t\t\t\t\t         of the process executing the shell is shown\n");
	printf(" setpriority [PID] <value>\t\t      Change the priority of process [PID] to <value>; if only one\n");
	printf("\t\t\t\t\t         argument is given the shell’s priority will be changed to <value>\n");

	printf("\n getuid\t\t\t\t\t      Prints the real and effective user credentialas of the process\n");
	printf("\t\t\t\t\t         running the shell\n");
	printf(" setuid [-l] <id>\t\t\t      Establishes the efective user id of the shell process; <id> represents\n");
	printf("\t\t\t\t\t         the UID; if -l is given <id> represents the login\n");

	printf("\n fork \t\t\t\t\t      The shell creates a child process (executing the same code as the\n");
	printf("\t\t\t\t\t          shell) and waits for it to end\n");
	printf(" execute <prog> [ARGS]... [@pri]\t      Executes, replacing the shell's code, the program <prog> with the arguments\n");
	printf("\t\t\t\t\t          [ARGS]; if [@pri] is given, change the process priority to <pri>\n");
	printf(" foreground <prog> [ARGS]... [@pri]\t      The shell creates a process that executes in foreground the program <prog>\n");
	printf("\t\t\t\t\t          with the arguments [ARGS]; if [@pri] is given, change the process\n");
	printf("\t\t\t\t\t          priority to <pri>\n");
	printf(" background <prog> [ARGS]... [@pri]\t      The shell creates a process that executes in background the program <prog>\n");
	printf("\t\t\t\t\t          with the arguments [ARGS]; if [@pri] is given, change the process\n");
	printf("\t\t\t\t\t          priority to <pri>\n");

	printf("\n run-as <login> <prog> [ARGS]... [@pri] [&]   Creates a process that executes as user <login> the program <prog>\n");
	printf("\t\t\t\t\t          and arguments [ARGS]; if [&] is given, the process will be executed in\n");
	printf("\t\t\t\t\t          background instead of foreground\n");
	printf(" execute-as <login> <prog> [ARGS]... [@pri]   Executes as user <login>, replacing the shells's code, the program <prog>\n");
	printf("\t\t\t\t\t          and arguments [ARGS]; program priority can be changed with <pri>\n");

	printf("\n listprocs\t\t\t\t      Shows the list of background processes of the shell\n");
	printf("\n proc [-fg] <PID>\t\t\t      Shows information on process <PID>; if [-fg] is given, the process <PID>\n");
	printf("\t\t\t\t\t          will be brought to the foreground\n");
	printf(" deleteprocs [OPTION]\t\t\t      Removes processes from the list depending on [OPTION]\n");
	printf("    -term\t\t\t\t        Removes from the list the processes that have exited normally\n");
	printf("    -sig\t\t\t\t        Removes from the list the processes that have been terminated by a signal\n");

	printf("\n <prog> [ARGS]... [@pri] [&]\t\t      If [&] is not given it behaves exactly as foreground command, else as\n");
	printf("\t\t\t\t\t          background do; alternatively the priority of the process can be\n");
		printf("\t\t\t\t\t          changed with <pri>\n");	

}

void fileSysHelp() {
	printf("\n\033[3m------------------------------\n    FILE-SYSTEM OPERATIONS\n------------------------------\033[0m\n\n");
	printf(" create [-dir] <name>\t\t\t     Creates an empty file with the name <name>; if [-dir] is specified\n");
	printf("\t\t\t\t\t        a directory is created\n");
	printf(" delete [-rec] <name1> <name2>...\t     Delete files or empty directories <name1>, <name2>...; if [-rec]\n");
	printf("\t\t\t\t\t        is specified, it will also delete non-empty directories\n");

	printf(" list [OPTIONS]... <name1> <name2>...        List information about <name1> <name2>... depending on the [OPTIONS]\n");
	printf("    -long\t\t\t\t       Long listing\n");
	printf("    -hid\t\t\t\t       List hidden elements\n");
	printf("    -dir\t\t\t\t       List the elements inside the directory specified not recursively\n");
	printf("    -rec\t\t\t\t       Combined with [-dir], list information about all the elements inside\n");
	printf("\t\t\t\t\t          a directory recursively\n");
}


void utilitiesHelp() {
	printf("\n\033[3m----------------------------\n    UTILITIES OPERATIONS\n----------------------------\033[0m\n\n");
	printf(" authors [-l|-n]\t\t\t     Shows the names and logins of the program authors; [-l] prints only\n");
	printf("\t\t\t\t\t         the logins [-n] prints only the names\n");
	printf(" getpid\t\t\t\t\t     Shows the pid of the process executing the shell\n");
	printf(" getppid\t\t\t\t     Shows the pid of the shell’s parent process\n");
	printf(" pwd\t\t\t\t\t     Shows the shell’s current workin directory\n");
	printf(" chdir <dir>\t\t\t\t     Changes the current working directory of the shell to <dir>\n");
	printf(" date\t\t\t\t\t     Shows the current date\n");
	printf(" time\t\t\t\t\t     Shows the current time\n");

	printf("\n historic [OPTION]\t\t\t     Handles the historic list depending on the [OPTION]; if no [OPTION]\n");
	printf("\t\t\t\t\t        is given, it shows all the historic\n");

	printf("    -c\t\t\t\t\t       Clears the historic\n");
	printf("    -r<num>\t\t\t\t       Repeats the command of the position <num> in the historic\n");
	printf("    -<num>\t\t\t\t       Shows the first <num> commands of the historic\n");

	printf("\n quit/exit/end\t\t\t\t     Exits the shell\n");
}

void shellHelp() {
	utilitiesHelp();
	fileSysHelp();
	memoryHelp();
	processHelp();
}
