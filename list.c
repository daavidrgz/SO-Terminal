#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdbool.h>

#define STRMAX 1024

void listDirFile(char* segmentedCommand[]);
void printPath(char* path, bool islong, bool ishide, bool isrec, int count);
void unitsize(float size, char unitsize[STRMAX]);
void filePrint(char* path, char* name, bool islong);
char* ConvierteModo (mode_t m, char* permisos);
char LetraTF(mode_t m);
int listArgumentType(char* segmentedCommand[], bool arguments[4]);

int main(int argc, char *argv[]) {
    listDirFile(argv);
    return 0;
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
                                        printf("\033[0;31mCannot open %s: %s\033[0m\n", aux, strerror(errno));
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
            strftime(time, 50, "%a %b %d %G", localtime(&statbuf.st_mtime));
            printf("\033[0;33m%s \033[0;36m%8ld %5s %5s %s \033[0;33m%10s (%ld) ", time, statbuf.st_ino,
                uidbuf->pw_name, gidbuf->gr_name, ConvierteModo(statbuf.st_mode, permisos),
                size, statbuf.st_nlink);
            //Statment to print the files and the directories with different colors.
            if ( typeoffile == 'd' ) {
                i = strlen(name) - 1;
                if ( name[i] == '/') {
                    name[i] = '\0';
                }
                printf("\033[1;32m%s/\033[0m", name);
            } else {
                printf("\033[1;34m%s\033[0m", name);
            }
            //If the file is a link, we add the name of its pointed file.
            if ( typeoffile == 'l' ) {
                readlink(path, linkedname, STRMAX);
                printf("\033[1;34m -> %s\033[0m", linkedname);
            }

        } else {
            if ( typeoffile == 'd' ) {
                i = strlen(name) - 1;
                if ( name[i] == '/') {
                    name[i] = '\0';
                }
                printf("\033[1;32m%s/", name);
            } else {
                printf("\033[1;34m%s", name);
            }
            printf("\033[0;33m (%s)\033[0m", size);
        }
        printf("\n");

    } else {
        printf("\033[0;31mCannot access %s: %s\033[0m\n", path, strerror(errno));
    }
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
