#ifndef PIPELINE_H
#define PIPELINE_H

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <fcntl.h>
#include <string>

using namespace std;

extern int childpid;

struct RedirectionInfo {
    string infile;
    string outfile;
    bool has_input;
    bool has_output;
    bool append;
};

RedirectionInfo parse_redirections(vector<string>& words) {
    RedirectionInfo redir;
    redir.has_input = false;
    redir.has_output = false;
    redir.append = false;
    
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i] == ">") {
            if (words.size() <= i + 1) {
                cerr << "Invalid redirection syntax" << endl;
                continue;
            }
            redir.outfile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            redir.has_output = true;
            redir.append = false;
            i--;
        } else if (words[i] == ">>") {
            if (words.size() <= i + 1) {
                cerr << "Invalid redirection syntax" << endl;
                continue;
            }
            redir.outfile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            redir.has_output = true;
            redir.append = true;
            i--;
        } else if (words[i] == "<") {
            if (words.size() <= i + 1) {
                cerr << "Invalid redirection syntax" << endl;
                continue;
            }
            redir.infile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            redir.has_input = true;
            i--;
        }
    }
    
    return redir;
}

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
        RedirectionInfo redir = parse_redirections(commands[i]);
        vector<char*> args;
        for (auto &str : commands[i]) {
            args.push_back(const_cast<char*>(str.c_str()));
        }
        args.push_back(NULL);
        
        if (args.size() == 1) continue;
        
        pid_t pid = fork();
        
        if (pid == 0) {
            if (redir.has_input) {
                int infd = open(redir.infile.c_str(), O_RDONLY);
                if (infd < 0) {
                    perror("open input file");
                    exit(1);
                }
                dup2(infd, STDIN_FILENO);
                close(infd);
            } else if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (redir.has_output) {
                int flags = O_CREAT | O_WRONLY;
                if (redir.append)
                    flags |= O_APPEND;
                else
                    flags |= O_TRUNC;
                
                int outfd = open(redir.outfile.c_str(), flags, 0644);
                if (outfd < 0) {
                    perror("open output file");
                    exit(1);
                }
                dup2(outfd, STDOUT_FILENO);
                close(outfd);
            } else if (i < num_cmds - 1) {
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