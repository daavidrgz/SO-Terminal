#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <stdio.h>
#include <signal.h>
#include <string.h>

struct SEN {
    char* nombre;
    int senal;
};

char* sig2Str(int sig);
int str2Sig(char* name);

#endif