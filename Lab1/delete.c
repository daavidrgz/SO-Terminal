#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>

#define STRMAX 1024

int deleteArgumentType(char* segmentedCommand[]);
void deleteDirFile(char* segmentedCommand[]);
void deleteRecursive(char* path);
void printPath(char* path);
void filePrint(char* path, char* name);
void unitsize(float size, char unitsize[STRMAX]);
char LetraTF(mode_t m);

int main(int argc, char *argv[]) {
    deleteDirFile(argv);
    return 0;
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

void deleteDirFile(char* segmentedCommand[]) {
    int deleteArgument = deleteArgumentType(segmentedCommand);
    int i;
    struct stat statbuf;

    if ( deleteArgument == 0 ) { //Short List the current directory
       printPath("."); 

    } else if ( deleteArgument == 1 ) { //Delete recursive
        i = 2;
        while ( segmentedCommand[i] != NULL ) {
            if ( lstat(segmentedCommand[i], &statbuf) == 0 ) {
                if ( LetraTF(statbuf.st_mode) != 'd' ) {
                    if ( unlink(segmentedCommand[i]) == 0 ) {
                        printf("\033[0;31mDeleted: %s\033[0m\n", segmentedCommand[i]);
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
                        printf("\033[0;31mDeleted: %s\033[0m\n", segmentedCommand[i]);
                    } else {
                        printf("Can't delete file: %s\n", strerror(errno));
                    }
                } else {
                    if ( rmdir(segmentedCommand[i]) == 0 ) {
                        printf("\033[0;31mDeleted: %s (dir)\033[0m\n", segmentedCommand[i]);
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
                            printf("\033[0;31mDeleted: %s\033[0m\n", aux);
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
        printf("\033[0;31mDeleted: %s (dir)\033[0m\n", path);
    } else {
        printf("Cannot delete directory: %s\n", strerror(errno));
    }
    closedir(directory);
}

void printPath(char* path) {
    struct dirent * elem;
    DIR* maindirectory;
    DIR* testdir;
    char aux[STRMAX];
    struct stat statbuf;

    if ( (testdir = opendir(path)) != NULL ) {
        for ( int j = 0; j<=1; j++ ) { //We scan two times the same folder; the first time we only print the files, and the second time we only print the folders.
            maindirectory = opendir(path);
            elem = readdir(maindirectory);
            while ( elem != NULL ) { //readdir() returns NULL when there are no more files in the directory
                strcpy(aux, path);
                if ( (strcmp(elem->d_name, ".") != 0) && (strcmp(elem->d_name, "..") != 0) ) {

                    if(aux[strlen(aux) - 1] == '/') { //We manage the relative routes
                        strcat(aux, elem->d_name);
                    } else {
                        strcat(strcat(aux, "/"), elem->d_name);
                    }

                    if ( lstat(aux, &statbuf) == -1 ) { //We get the info about the file or dir
                        printf("Cannot access %s: %s\n", aux, strerror(errno));
                    } else {

                        if ( (LetraTF(statbuf.st_mode) == 'd' && j == 1) || (LetraTF(statbuf.st_mode) != 'd' && j == 0) ) {
                            filePrint(aux, elem->d_name);
                        }
                    }                    
                }

                elem = readdir(maindirectory);
            }
            
            closedir(maindirectory);
        }

    } else {
        printf("\033[0;31mCannot open %s: %s\033[0m\n", path, strerror(errno));
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

void filePrint(char* path, char* name) {
    char size[STRMAX];
    struct stat statbuf;
    char typeoffile;

    lstat(path, &statbuf);
    typeoffile = LetraTF(statbuf.st_mode);
    unitsize((float)statbuf.st_size, size);

    if ( typeoffile == 'd' ) {
        printf("\033[1;32m%s/", name);
    } else {
        printf("\033[1;34m%s", name);
    }
    printf("\033[0;33m (%s)\033[0m", size);
    printf("\n");
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
