#include "signal_handler.h"

static struct SEN sigstrnum[] = {
    {"HUP", SIGHUP},
    {"INT", SIGINT},
    {"QUIT", SIGQUIT},
    {"ILL", SIGILL},
    {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT},
    {"IOT", SIGIOT},
    {"BUS", SIGBUS},
    {"FPE", SIGFPE},
    {"KILL", SIGKILL},
    {"USR1", SIGUSR1},
    {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2},
    {"PIPE", SIGPIPE},
    {"ALRM", SIGALRM},
    {"TERM", SIGTERM},
    {"CHLD", SIGCHLD},
    {"CONT", SIGCONT},
    {"STOP", SIGSTOP},
    {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN},
    {"TTOU", SIGTTOU},
    {"URG", SIGURG},
    {"XCPU", SIGXCPU},
    {"XFSZ", SIGXFSZ},
    {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF},
    {"WINCH", SIGWINCH},
    {"IO", SIGIO},
    {"SYS", SIGSYS},
   
#ifdef SIGPOLL
    {"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
    {"PWR", SIGPWR},
#endif
#ifdef SIGEMT
    {"EMT", SIGEMT},
#endif
#ifdef SIGINFO
    {"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
    {"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
    {"CLD", SIGCLD},
#endif
#ifdef SIGLOST
    {"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
    {"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
    {"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
    {"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
    {"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
    {"WAITING", SIGWAITING},
#endif
    {NULL,-1}
};

char* sig2Str(int sig) {
    int i;
    for ( i = 0; sigstrnum[i].nombre != NULL; i++ )
        if ( sig == sigstrnum[i].senal )
            return sigstrnum[i].nombre;
    return "SIGUNKNOWN";
}

int str2Sig(char* name) {
    for ( int i = 0; sigstrnum[i].nombre != NULL; i++ )
        if ( !strcmp(name, sigstrnum[i].nombre) )
            return sigstrnum[i].senal;
    return -1;
}
