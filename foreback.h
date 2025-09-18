#ifndef BGFG_H
#define BGFG_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

int childpid = -1;

void foreback(vector<string> words) {
    vector<char*> args;
    bool background = (!words.empty() && words.back()=="&");
    for (auto &str: words) {
        if (str == "&") continue;
        args.push_back(const_cast<char*>(str.c_str()));
    }
    args.push_back(NULL);

    pid_t child_pid = fork();
    if (child_pid == 0) {
        int result = execvp(args[0], args.data());
        if (result == -1) {
            perror("Failed to execute");
            return;
        }
    }
    else if (child_pid > 0) {
        if (background) {
            cout << child_pid << endl;
        }
        else {
            int status;
            childpid = child_pid;
            waitpid(child_pid, &status, 0);
        }
    }
    else {
        perror ("for failed");
    }
}

#endif