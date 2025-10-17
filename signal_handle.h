#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

extern int childpid;

void sigtstp_handler(int sig) {
    if (childpid > 0) {
        kill(childpid, SIGTSTP);
        childpid = -1;
    }
}

void sigint_handler(int sig) {
    if (childpid > 0) {
        kill(childpid, SIGINT);
        childpid = -1;
    }
}

void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
        } else if (WIFSIGNALED(status)) {
        }
    }
}

#endif