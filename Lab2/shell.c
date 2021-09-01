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
#include "linked_list.h"
#include "memory_list.h"

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
void manageHistoric(LinkedList* listHead, char * segmentedCommand[]);
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

//Utilities
char* ConvierteModo(mode_t m, char *permisos);
char LetraTF(mode_t m);
void unitsize(float size, char unitsize[STRMAX]);
int trocearCadena(char* cadena, char* trozos[]);
void processCommand(LinkedList* listHead, char * segmentedCommand[]);


int main() {
	createEmptyMemList(&memList);
    bool finished = false;
    char * username = getenv("USER");
    char * segmentedCommand[STRMAX];
    LinkedList historic;
    char host[STRMAX];
    char lastCommand[STRMAX];
    char commandInit[STRMAX];
    char current_directory[STRMAX];

    createEmptyList(&historic);
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
        if ( trocearCadena(lastCommand,segmentedCommand) != 0 ) {
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
                    processCommand(&historic, segmentedCommand);
                } else {
                    if( !insertItemList(historic, NULL, commandInit) ) {
                        printf("Error: Imposible to save historic");
                    }

                    processCommand(&historic, segmentedCommand);
                }
            }
        }
    } while (!finished);
}

int listArgumentType(char* segmentedCommand[], bool arguments[4]) {
    int i = 1, j = 0;
    bool fin = true;

    while ( i < 5 && segmentedCommand[i] != NULL && fin) { //Loop to determine which arguments were recived
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
    if ( segmentedCommand[1] == NULL){
        return 0;
    }
    if ( segmentedCommand[2] == NULL ) {
        return 1;
    }
    if ( segmentedCommand[3] != NULL){
        return -2;

    } else if ( strcmp(segmentedCommand[1], "-dir") == 0 ) {
        return 2;
    }
    return -1;
}

int deleteArgumentType(char* segmentedCommand[]) {
    if ( segmentedCommand[1] == NULL){
        return 0;
    }
    if ( strcmp(segmentedCommand[1], "-rec") == 0 ) {
        if ( segmentedCommand[2] == NULL ) {
            return 0;
        } else {
            return 1;
        }
    }
    return 2;
}

int historicArgumentType(char * segmentedCommand[]) {
    char * argument = segmentedCommand[1];

    if ( argument == NULL ) {
        return 0;
    } else {
        if( segmentedCommand[2] != NULL ) {
            return -2;
        } else if ( argument[0] == '-' ) {
            if ( argument[1] == 'c' ){
                if ( argument[2] == '\0' ) {
                    return 1;
                }
            } else if ( argument[1] == 'r'  ) {
                if( argument[2] >= '0' && argument[2] <= '9' ) {
                    return 2;
                }
            } else if ( argument[1] >= '0' && argument[1] <= '9' ) {
                return 3;
            }
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

void printAuthors(char * segmentedCommand[]) {
    if ( segmentedCommand[1] != NULL ) {
        if ( segmentedCommand[2] == NULL){
            if ( strcmp(segmentedCommand[1], "-l" ) == 0) {
                printf("leopoldo.estevez\ndavid.rbacelar\n");

            } else if ( strcmp(segmentedCommand[1], "-n") == 0 )  {
                printf("Leopoldo Estevez\nDavid Rodríguez\n");

            } else {
                printf("Error: %s\n", strerror(EINVAL));
            }

        } else {
            printf("Error: %s\n", strerror(E2BIG));
        }
    } else {
        printf("Leopoldo Estevez: leopoldo.estevez \nDavid Rodríguez: david.rbacelar \n");
    }
}

void changeDirectory(char * segmentedCommand[]) {
    char cwd[STRMAX];

    if ( segmentedCommand[1] != NULL ) {
        if ( segmentedCommand[2] == NULL ) {
            if ( chdir(segmentedCommand[1]) != 0 ) {
                printf("Error: %s\n", strerror(errno));
            }

        } else {
            printf("Error: %s\n", strerror(E2BIG));
        }

    }
    else{
        if ( getcwd(cwd, sizeof(cwd)) != NULL ) {
            printf("Current working dir: %s\n", cwd);
        }
    }
}

void manageHistoric(LinkedList* listHead, char * segmentedCommand[]) {
    char * argument = segmentedCommand[1];
    int historicArgument = historicArgumentType(segmentedCommand);

    if ( historicArgument == -2 ) {
        printf("Error: %s\n", strerror(E2BIG));

    } else if ( historicArgument == -1 ) {
        printf("Error: %s\n", strerror(EINVAL));

    } else if ( historicArgument == 0 ) { //Print all historic
        printList(*listHead, -1);

    } else if ( historicArgument == 1 ) { //Clear historic
        if(strcmp(argument, "-c") == 0 ) {
            clearList(*listHead);
            printf("The history has been cleared\n");
        } else {
            printf("Error: %s\n", strerror(EINVAL));
        }

    } else if ( historicArgument == 2 ) { //Repeat historic entry
        int argumentNumericValue = atoi(&argument[2]);
        char * fetchedCommand = getItem(*listHead, argumentNumericValue);
        if ( fetchedCommand != NULL ) {
            char commandInit[STRMAX];
            strcpy(commandInit, fetchedCommand);
            printf("Running the command (%d)> %s", argumentNumericValue, commandInit);
            trocearCadena(commandInit, segmentedCommand);
            processCommand(listHead, segmentedCommand);
        } else {
            printf("Error: Index out of bounds\n");
        }

    } else { //Print a number of historic entries
        int maxCounted = atoi(&argument[1]);
        if( maxCounted == 0 ) {
            if( argument[1] != '0' ) {
                printf("Error: %s\n", strerror(EINVAL));
            }
        } else {
            printList(*listHead, maxCounted);
        }
    }
}

void createDirFile(char* segmentedCommand[]) {
    int createArgument= createArgumentType(segmentedCommand);

    if ( createArgument == -2 ) {
        printf("Error: %s\n", strerror(E2BIG));

    } else if ( createArgument == -1 ) {
        printf("Error: %s\n", strerror(EINVAL));

    } else if ( createArgument == 0 ) { //Short List the current directory
        printPath(".", false, true, false, 0);

    } else if ( createArgument == 1 ) { //Create a file
        if ( open(segmentedCommand[1], O_CREAT | O_EXCL, 0777) == -1 ) {
            printf("Cannot create file %s: %s\n", segmentedCommand[1], strerror(errno));
        } else {
            printf("Created: %s (file)\n", segmentedCommand[1]);
        }

    } else { //Create a directory
        if ( mkdir(segmentedCommand[2], 0777) == -1 ) {
            printf("Cannot create directory %s: %s\n", segmentedCommand[2], strerror(errno));
        } else {
            printf("Created: %s (dir)\n", segmentedCommand[2]);
        }
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
                    if ( unlink(segmentedCommand[i]) == 0 ) {
                        printf("%sDeleted: %s%s\n", RED, segmentedCommand[i], WHITE);
                    } else {
                        printf("Cannot delete file %s: %s\n", segmentedCommand[i], strerror(errno));
            }

                } else {
                    deleteRecursive(segmentedCommand[i]);
                }
            } else {
                printf("Cannot access %s: %s\n", segmentedCommand[i], strerror(errno));
            }
            i++;
        }

    } else { //Normal delete
        i = 1;
        while ( segmentedCommand[i] != NULL ) {
            if ( lstat(segmentedCommand[i], &statbuf) == 0 ) {
                if ( LetraTF(statbuf.st_mode) != 'd' ) {
                    if ( unlink(segmentedCommand[i]) == 0 ) {
                        printf("%sDeleted: %s%s\n", RED, segmentedCommand[i], WHITE);
                    } else {
                        printf("Can't delete file: %s\n", strerror(errno));
                    }
                } else {
                    if ( rmdir(segmentedCommand[i]) == 0 ) {
                        printf("%sDeleted: %s (dir)%s\n", RED, segmentedCommand[i], WHITE);
                    } else {
                        printf("Cannot delete directory: %s\n", strerror(errno));
                    }
                }

            } else {
                printf("Cannot access %s: %s\n", segmentedCommand[i], strerror(errno));
            }
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
                if ( aux[strlen(aux) - 1] == '/' ) { //We manage the relative routes
                    strcat(aux, elem->d_name);
                } else {
                    strcat(strcat(aux, "/"), elem->d_name);
                }

                if ( lstat(aux, &statbuf) == 0 ) {
                    if ( LetraTF(statbuf.st_mode) == 'd' ) {
                        deleteRecursive(aux); //Recursive call
                    } else {
                        if ( unlink(aux) == 0 ) {
                            printf("%sDeleted: %s%s\n", RED, aux, WHITE);
                        } else {
                            printf("Cannot delete file: %s\n", strerror(errno));
                        }
                    }

                } else {
                    printf("Cannot access %s: %s", aux, strerror(errno));
                }
            }

        elem = readdir(directory);
        }
    }

    if ( rmdir(path) == 0 ) {
        printf("%sDeleted: %s (dir)%s\n", RED, path, WHITE);
    } else {
        printf("Cannot delete directory: %s\n", strerror(errno));
    }
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
            printf("No directory specified\n");
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
                printf("Cannot access %s: %s\n", segmentedCommand[ppath], strerror(errno));
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
                        printf("Cannot access %s: %s\n", aux, strerror(errno));

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
                                        printf("%sCannot open %s: %s%s\n", RED, aux, strerror(errno), WHITE);
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
        printf("%sCannot open %s: %s%s\n", RED, path, strerror(errno), WHITE);
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

void filePrint(char* path, char* name, bool islong) {
    char permisos[STRMAX], linkedname[STRMAX], size[STRMAX];
    struct stat statbuf;
    char typeoffile;
    char time[50];
    struct passwd* uidbuf;
    struct group* gidbuf;
    int i = 0;

    if ( lstat(path, &statbuf) == 0 ) {
        typeoffile = LetraTF(statbuf.st_mode);
        unitsize((float)statbuf.st_size, size);
        if ( islong ) {
            uidbuf = getpwuid(statbuf.st_uid);
            gidbuf = getgrgid(statbuf.st_gid);
            strftime(time, 50, "%b %d %H:%M", localtime(&statbuf.st_mtime));
            printf("%s%s %s%8ld %5s %5s %s %s%10s (%ld) ", PINK, time, BLUE, statbuf.st_ino,
                uidbuf->pw_name, gidbuf->gr_name, ConvierteModo(statbuf.st_mode, permisos),
                PINK, size, statbuf.st_nlink);
            //Statment to print the files and the directories with different colors.
            if ( typeoffile == 'd' ) {
                i = strlen(name) - 1;
                if ( name[i] == '/') {
                    name[i] = '\0';
                }
                printf("%s%s/%s", GREEN, name, WHITE);
            } else {
                printf("%s%s%s", CYAN, name, WHITE);
            }
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
            } else {
                printf("%s%s", CYAN, name);
            }
            printf("%s (%s)%s", PINK, size, WHITE);
        }
        printf("\n");

    } else {
        printf("%sCannot access %s: %s%s\n", RED, path, strerror(errno), WHITE);
    }
}

void memoryOperation(char* segmentedCommand[]) {
	if ( segmentedCommand[1] == NULL ) {
		printMemList(memList, NULL);
	} else if ( strcmp(segmentedCommand[1], "-allocate") == 0 ) {
		memoryAllocate(segmentedCommand);
	} else if ( strcmp(segmentedCommand[1], "-dealloc") == 0 ) {
		memoryDeallocate(segmentedCommand);
	} else if ( strcmp(segmentedCommand[1], "-deletekey") == 0 ) {
		if ( segmentedCommand[2] == NULL ) {
			printf("Too few arguments\n");
		} else if ( segmentedCommand[3] != NULL ) {
			printf("%s\n", strerror(E2BIG));
		} else
			memoryDeleteKey(segmentedCommand[2]);
	} else if ( strcmp(segmentedCommand[1], "-show") == 0 ) {
		memoryShow(segmentedCommand);
	} else if ( strcmp(segmentedCommand[1], "-show-vars") == 0 ) {
		memoryShowVarFun(segmentedCommand);
	} else if ( strcmp(segmentedCommand[1], "-show-funcs") == 0 ) {
		memoryShowVarFun(segmentedCommand);
	} else if ( strcmp(segmentedCommand[1], "-dopmap") == 0 ) {
		memoryPmap();
	} else {
		printf("%s\n", strerror(EINVAL));
	}
}

void memoryAllocate(char* segmentedCommand[]) {
	if ( segmentedCommand[2] == NULL ) {
		printMemList(memList, NULL);
	} else if ( strcmp(segmentedCommand[2], "-malloc") == 0 ) {
		memoryAllocateMalloc(segmentedCommand);
	} else if ( strcmp(segmentedCommand[2], "-mmap") == 0 ) {
		memoryAllocateMmap(segmentedCommand);
	} else if ( strcmp(segmentedCommand[2], "-createshared") == 0 ) {
		memoryAllocateCreateShared(segmentedCommand);
	} else if ( strcmp(segmentedCommand[2], "-shared") == 0 ) {
		memoryAllocateShared(segmentedCommand);
	} else {
		printf("%s\n", strerror(EINVAL));
	}
}

void memoryDeallocate(char* segmentedCommand[]) {
	if ( segmentedCommand[2] == NULL ) {
		printMemList(memList, NULL);
	} else if ( strcmp(segmentedCommand[2], "-malloc") == 0 ) {
		memoryDeallocMalloc(segmentedCommand);
	} else if ( strcmp(segmentedCommand[2], "-mmap") == 0 ) {
		memoryDeallocMmap(segmentedCommand);
	} else if ( strcmp(segmentedCommand[2], "-shared") == 0 ) {
		memoryDeallocShared(segmentedCommand);
	} else {
		memoryDeallocAddr(segmentedCommand);
	}
}

void memoryShow(char* segmentedCommand[]) {
	int local1 = 0;
	char local2 = 'a';
	float local3 = 1.2;

	if ( segmentedCommand[2] == NULL ) {
		printf("Local variables:      %p,   %p,   %p\n", &local1, &local2, &local3);
		printf("Global variables:     %p,   %p,   %p\n", &global1, &global2, &global3);
		printf("Program functions:    %p,   %p,   %p\n", &processCommand, &memoryOperation, &memoryShowVarFun);
	} else if ( segmentedCommand[3] != NULL ) {
		printf("%s\n", strerror(E2BIG));
	} else if ( strcmp(segmentedCommand[2], "-malloc") == 0 ) {
		printMemList(memList, "malloc");
	} else if ( strcmp(segmentedCommand[2], "-mmap") == 0 ) {
		printMemList(memList, "mmap");
	} else if ( strcmp(segmentedCommand[2], "-shared") == 0 ) {
		printMemList(memList, "shared");
	} else if ( strcmp(segmentedCommand[2], "-all") == 0 ) {
		printMemList(memList, NULL);
	} else {
		printf("%s\n", strerror(EINVAL));
	}
	
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
		printf("%s\n", strerror(E2BIG));
		return;
	}

	if ( (size_t) (size = atol(segmentedCommand[3])) == 0 ) {
		printf("%s\n", strerror(EINVAL));
		return;
	}
	if ( (p = malloc(size)) == NULL) {
		printf("Cannot allocate %s bytes of memory\n", segmentedCommand[3]);
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
		printf("Cannot open %s: %s\n", segmentedCommand[3], strerror(errno));
		return;
	}
	if ( lstat(segmentedCommand[3], &statbuf) == -1 ) {
		printf("Cannot access %s: %s\n", segmentedCommand[3], strerror(errno));
		return;
	}
	if ( (p = mmap(NULL, statbuf.st_size, protection, map, fd, 0)) == MAP_FAILED ) {
		printf("Cannot map file %s: %s\n", segmentedCommand[3], strerror(errno));
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
		printf("Too few arguments\n");
		return;
	}
	if ( segmentedCommand[5] != NULL ) {
		printf("%s\n", strerror(E2BIG));
		return;
	}
	if ( (k = (key_t) atoi(segmentedCommand[3])) == 0 || k == IPC_PRIVATE ) {
		printf("%s\n", strerror(EINVAL));
		return;
	}
	if ( (tam = (size_t) atoll(segmentedCommand[4])) == 0 && strcmp(segmentedCommand[4], "0") != 0 ) {
		printf("%s\n", strerror(EINVAL));
		return;
	}
	if ( tam ) flags = flags | IPC_CREAT | IPC_EXCL;
	if ( (id = shmget(k, tam, flags) ) == -1 ) {
		printf("Cannot create shared memory (Key: %d, Size: %ld): %s\n", k, tam, strerror(errno));
		return;
	}
	if ( (p = shmat(id, NULL, 0)) == (void*) -1 ) {
		printf("Cannot asign shared memory (Key: %d, Size: %ld): %s\n", k, tam, strerror(errno));
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
	if ( segmentedCommand[3] == NULL ) {
		printMemList(memList, "shared");

	} else if ( segmentedCommand[4] != NULL ) {
		printf("%s\n", strerror(E2BIG));

	} else {
		segmentedCommand[4] = "0";
		segmentedCommand[5] = NULL;
		memoryAllocateCreateShared(segmentedCommand);
	}
}

void memoryDeallocMalloc(char* segmentedCommand[]) {
	size_t size;
	PosM p;

	if ( segmentedCommand[3] == NULL ) {
		printMemList(memList, "malloc");

	} else if ( segmentedCommand[4] != NULL ) {
		printf("%s\n", strerror(E2BIG));

	} else {
		if ( (size = (size_t) atoll(segmentedCommand[3])) == 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
		if ( (p = getMallocItem(memList, size)) == NULL ) {
			printf("Cannot find memory allocated with malloc of size %ld\n", size);
			return;
		}
		printf("%sDeallocated %ld bytes at %p%s\n", RED, size, p->pointer, WHITE);
		free(p->pointer);
		deleteItemMemList(memList, p);
	}
}

void memoryDeallocMmap(char* segmentedCommand[]) {
	PosM p;

	if ( segmentedCommand[3] == NULL ) {
		printMemList(memList, "mmap");

	} else if ( segmentedCommand[4] != NULL ) {
		printf("%s\n", strerror(E2BIG));

	} else {
		if ( (p = getMmapItem(memList, segmentedCommand[3])) == NULL ) {
			printf("Cannot find mapped file %s\n", segmentedCommand[3]);
			return;
		}
		close(p->fd);
		if ( munmap(p->pointer, p->size) == -1 ) {
			printf("Cannot unmap file %s: %s\n", segmentedCommand[3], strerror(errno));
			return;
		}
		printf("%sUnmaped file %s at %p%s\n", RED, segmentedCommand[3], p->pointer, WHITE);
		deleteItemMemList(memList, p);
	}
}

void memoryDeallocShared(char* segmentedCommand[]) {
	PosM p;
	key_t key;

	if ( segmentedCommand[3] == NULL ) {
		printMemList(memList, "shared");

	} else if ( segmentedCommand[4] != NULL ) {
		printf("%s\n", strerror(E2BIG));

	} else {
		if ( (key = (key_t) atoi(segmentedCommand[3])) == 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
		if ( (p = getSharedItem(memList, key)) == NULL ) {
			printf("Cannot find shared memory (Key: %d)\n", key);
			return;
		}
		if ( shmdt(p->pointer) == -1 ) {
			printf("Cannot deatach shared memory (Key: %d): %s\n", key, strerror(errno));
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
		printf("%s\n", strerror(E2BIG));
		return;
	}
	dir = (void*)strtol(segmentedCommand[2], &endptr, 16);
	if ( *endptr != '\0') {
		printf("%s\n", strerror(EINVAL));
		return;
	}
	if ( (p = searchAddr(memList, dir)) == NULL ) {
		printf("Cannot find adrress %p\n", dir);
		return;
	}
	if ( strcmp(p->type, "malloc") == 0 ) {
		printf("%sDeallocated %ld bytes at %p%s\n", RED, p->size, p->pointer, WHITE);
		free(p->pointer);

	} else if ( strcmp(p->type, "mmap") == 0 ) {
		close(p->fd);
		if ( munmap(p->pointer, p->size) == -1 ) {
			printf("Cannot unmap file %s: %s\n", p->filename, strerror(errno));
			return;
		}
		printf("%sUnmaped file %s at %p%s\n", RED, p->filename, p->pointer, WHITE);

	} else {
		if ( shmdt(p->pointer) == -1 ) {
			printf("Cannot deatach shared memory (Key: %d): %s\n", p->key, strerror(errno));
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
		printf ("Cannot delete key %s\n", key);
		return;
	}
	if ( (id = shmget(clave, 0, 0666)) == -1 ) {
		printf("Cannot get shared memory (Key: %d): %s\n", clave, strerror(errno));
		return;
	}
	if ( shmctl( id, IPC_RMID, NULL) == -1 ) {
		printf("Cannot remove shared memory (Key: %d): %s\n", clave, strerror(errno));
		return;
	}
	printf("%sRemoved: Key %d%s\n", RED, clave, WHITE);
}

void memoryPmap() {
	pid_t pid;
	char elpid[32];
	char* argv[3] = {"pmap", elpid, NULL};
	sprintf(elpid, "%d", (int) getpid());

	if ( (pid = fork()) == -1 ) {
		printf("Cannot create the process\n");
	} else if ( pid == 0 ) {
		if ( execvp(argv[0],argv) == -1 )
			printf("Cannot execute pmap\n");
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
		printf("%s\n", strerror(E2BIG));
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
		printf("Too few arguments\n");
		return;
	}
	if ( segmentedCommand[2] == NULL )
		count = 25;

	else if ( segmentedCommand[3] != NULL ) {
		printf("%s\n", strerror(E2BIG));
		return;

	} else if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	ptr = (char*)strtol(segmentedCommand[1], &endptr, 16);
	if ( *endptr != '\0') {
		printf("%s\n", strerror(EINVAL));
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
		printf("Too few arguments\n");
	} else if ( segmentedCommand[2] == NULL ) {
		count = 128;
		byte  = 65;
	} else if ( segmentedCommand[3] == NULL ) {
		if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
		byte = 65;
	} else if ( segmentedCommand[4] != NULL) {
		printf("%s\n", strerror(E2BIG));
		return;
		
	} else {
		if ( (count = atoi(segmentedCommand[2])) == 0 && strcmp(segmentedCommand[2], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
		if ( (byte = (int) strtol(segmentedCommand[3], NULL, 16)) == 0 && strcmp(segmentedCommand[3], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
	}

	ptr = (char*)strtol(segmentedCommand[1], &endptr, 16);
	if ( *endptr != '\0') {
		printf("%s\n", strerror(EINVAL));
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

	if ( segmentedCommand[1] == NULL ) {
		printf("Too few arguments\n");
	} else if ( segmentedCommand[2] != NULL ) {
		printf("%s\n", strerror(E2BIG));
	} else {
		if ( (n = atoi(segmentedCommand[1])) == 0 && strcmp(segmentedCommand[1], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
		} else
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
		printf("Too few arguments\n");
		return;
	}
	if ( segmentedCommand[3] == NULL ) {
		n = LEERCOMPLETO;
	} else if ( segmentedCommand[4] != NULL ) {
		printf("%s\n", strerror(E2BIG));
		return;
	} else {
		if ( (n = atoi(segmentedCommand[3])) == 0 && strcmp(segmentedCommand[3], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
	}
	
	fich = segmentedCommand[1];
	tam = n;
	p = (void*)strtol(segmentedCommand[2], &endptr, 16);
	if ( *endptr != '\0') {
		printf("%s\n", strerror(EINVAL));
		return;
	}
	if ( lstat(fich, &s) == -1 || (df = open(fich, O_RDONLY)) == -1 ) {
		printf("Cannot read file: %s\n", strerror(errno));
		return;
	}
	if ( n == LEERCOMPLETO )
		tam = (ssize_t)s.st_size;
	if ( read(df, p, tam) == -1 ) {
		printf("Cannot read file: %s\n", strerror(errno));
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
		printf("Too few arguments\n");
		return;
	}
	else
		flags = flags | O_EXCL;
		
	if ( segmentedCommand[1+i] == NULL || segmentedCommand[2+i] == NULL ) {
		printf("Too few arguments\n");
		return;
	}
	if ( segmentedCommand[3+i] == NULL ) {
		n = LEERCOMPLETO;

	} else if ( segmentedCommand[4+i] != NULL ) {
		printf("%s\n", strerror(E2BIG));
		return;
	}

	if ( n != LEERCOMPLETO ) {
		if ( (n = atoi(segmentedCommand[3+i])) == 0 && strcmp(segmentedCommand[3+i], "0") != 0 ) {
			printf("%s\n", strerror(EINVAL));
			return;
		}
	}
	
	fich = segmentedCommand[1+i];
	tam = n;
	p = (void*)strtol(segmentedCommand[2+i], &endptr, 16);
	if ( *endptr != '\0') {
		printf("%s\n", strerror(EINVAL));
		return;
	}

	if ( (df = open(fich, flags, mode)) == -1 || lstat(fich, &s) == -1 ) {
		printf("Cannot open/create file: %s\n", strerror(errno));
		return;
	}
	if ( n == LEERCOMPLETO )
		tam = (ssize_t)s.st_size;
	if ( write(df, p, tam) == -1 ) {
		printf("Cannot write file: %s\n", strerror(errno));
	} else {
		printf("%sWrite file: Success%s\n", GREEN, WHITE);
	}
	close(df);
}


void processCommand(LinkedList* listHead, char * segmentedCommand[]) {
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

    } else if ( strcmp(segmentedCommand[0], "authors") == 0 ) {
        printAuthors(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "chdir") == 0 ) {
        changeDirectory(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "historic") == 0 ) {
        manageHistoric(listHead, segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "create") == 0 ) {
        createDirFile(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "delete") == 0 ) {
        deleteDirFile(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "list") == 0 ) {
        listDirFile(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "memory") == 0 ) {
        memoryOperation(segmentedCommand);

    } else if ( strcmp(segmentedCommand[0], "memdump") == 0 ) {
		memDump(segmentedCommand);

	} else if ( strcmp(segmentedCommand[0], "memfill") == 0 ) {
		memFill(segmentedCommand);

	} else if ( strcmp(segmentedCommand[0], "readfile") == 0 ) {
		readFile(segmentedCommand);

	} else if ( strcmp(segmentedCommand[0], "writefile") == 0 ) {
		writeFile(segmentedCommand);

	} else if ( strcmp(segmentedCommand[0], "recurse") == 0 ) {
		doRecursive(segmentedCommand);

	} else {
		printf("Error: %s\n", strerror(EPERM));
	}
}

int trocearCadena(char * cadena, char * trozos[]) {
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL) {
        return 0;
    }
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
