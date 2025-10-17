#ifndef PIPELINE_H
#define PIPELINE_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <fcntl.h>

using namespace std;

extern int childpid;

void execute_pipeline(vector<vector<string>>& commands) {
    int num_cmds = commands.size();
    int pipes[num_cmds - 1][2];
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }
    
    for (int i = 0; i < num_cmds; i++) {
        vector<char*> args;
        for (auto &str : commands[i]) {
            args.push_back(const_cast<char*>(str.c_str()));
        }
        args.push_back(NULL);
        
        pid_t pid = fork();
        
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(args[0], args.data());
            perror("execvp");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
            return;
        }
    }

    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }
}

#endif