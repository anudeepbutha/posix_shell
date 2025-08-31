#ifndef BGFG_H
#define BGFG_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

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
            cout << "Background process running: " << child_pid << endl;
        }
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