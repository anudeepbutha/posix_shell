#include <bits/stdc++.h>
#include <fcntl.h>
#include <iostream>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "autocomplete.h"
#include "changedir.h"
#include "foreback.h"
#include "history.h"
#include "pinfo.h"
#include "search.h"
#include "signal_handle.h"
#include "showlist.h"

using namespace std;

pid_t child_pid = 0;

char** command_suggestion(const char* text, int start, int end) {
    if (start == 0)
        return rl_completion_matches(text, command_generator);

    return NULL;
}

int main () {

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    passwd *userdetails = getpwuid(getuid());
    char* username = userdetails->pw_name;
    string homedir = userdetails->pw_dir;

    char hostname[1024];
    gethostname(hostname, sizeof(hostname));

    char home_path[1024];
    getcwd(home_path, sizeof(home_path));
    string histfile = homedir + "/.myshell_hist";
    read_history(histfile.c_str());

    if (history_length > 20)
        stifle_history(20);

    rl_attempted_completion_function = command_suggestion;

    while (1) {
        char current_path[1024];
        getcwd(current_path, sizeof(current_path));
        static char *line_read = (char *)NULL;

        string prompt;
        if (strcmp(current_path, home_path) == 0)
            prompt = string(username) + "@" + string(hostname) + ":~>";
        else
            prompt = string(username) + "@" + string(hostname) + ":" + string(current_path) + ">";

        line_read = readline(prompt.c_str());
        if (*line_read) add_history(line_read);

        vector <string> words;
        char* token = strtok(line_read, " ");
        while (token != NULL) {
            words.push_back(token);
            token = strtok(nullptr, " ");
        }

        string infile, outfile;
        int out_red= 0, in_red = 0, append = 0;;
        int outfd_dup = -1, infd_dup = -1;
        for (size_t i=0; i<words.size(); i++) {
            if (words[i] == ">") {
                if (words.size() <= i+1) {
                    cout << "Invalid Arguments" << endl;
                    break;
                }
                outfile = words[i+1];
                words.erase(words.begin()+i, words.begin()+i+2);
                out_red = 1; i-=1; append=0;
            }
            else if (words[i] == ">>") {
                if (words.size() <= i+1) {
                    cout << "Invalid Arguments" << endl;
                    break;
                }
                outfile = words[i+1];
                words.erase(words.begin()+i, words.begin()+i+2);
                out_red = 1; i-=1; append = 1;
            }
            else if (words[i] == "<") {
                if (words.size() <= i+1) {
                    cout << "Invalid Arguments" << endl;
                    break;
                }
                infile = words[i+1];
                words.erase(words.begin()+i, words.begin()+i+2);
                in_red = 1; i-=1;
            }
        }

        if (out_red == 1) {
            int flags;
            if (append == 1) 
                flags = O_CREAT | O_WRONLY | O_APPEND;
            else
                flags = O_CREAT | O_WRONLY | O_TRUNC;
            
            int outfd = open(outfile.c_str(), flags, 0644);
            if (outfd < 0)
                perror("Unable to open a file");
            else
            {
                outfd_dup = dup(STDOUT_FILENO);
                dup2(outfd, STDOUT_FILENO);
                close(outfd);
            }
        }

        if (in_red == 1) {
            int infd = open(infile.c_str(), O_RDONLY);
            if (infd < 0)
                perror("Unable to open a file");
            else {
                infd_dup = dup(STDIN_FILENO);
                dup2(infd, STDIN_FILENO);
                close(infd);
            }
        }

        if (words.size() < 1) continue;
        if(words[0] == "cd")
            change_directory(words,home_path,current_path);
        else if (words[0] == "echo")
            echo_print(words);
        else if ((words[0] == "pwd") && (words.size() == 1))
            cout << current_path << endl;
        else if (words[0] == "ls")
            showlist(words);
        else if (words[0] == "pinfo")
            process_info(words);
        else if (words[0] == "search")
            search(words);
        else if (words[0] == "history")
            print_history(words);
        else if (words[0] == "exit" && words.size() == 1) {
            free(line_read);
            write_history(histfile.c_str());
            return 0;
        }
        else
            foreback(words);

        if (out_red == 1) {
            dup2(outfd_dup, STDOUT_FILENO);
            close(outfd_dup);
            out_red = 0;
        }

        if (in_red == 1) {
            dup2(infd_dup, STDIN_FILENO);
            close(infd_dup);
            in_red = 0;
        }
        if (history_length > 20)
            remove_history(0);
        free(line_read);
    }
    write_history(histfile.c_str());
    return 0;
}