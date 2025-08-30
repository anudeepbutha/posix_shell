#ifndef BGFG_H
#define BGFG_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

void foreback(vector<string> words) {
    vector<char*> args;
    for (string str: words)
        args.push_back(&str[0]);
    args.push_back(NULL);
    pid_t child_pid = fork();
    if (child_pid == 0) {
        int result = execvp(args[0], args.data());
        if (result == -1)
            perror("Failed to execute");
    }
    else if (child_pid > 0) {
        if (words[1] == "&");
        else {
            int status;
            waitpid(child_pid, &status, 0);
            cout << "Process with id: " << child_pid << " is stopped" << endl;
        }
    }
    else {
        perror ("for failed");
    }
}

#endif