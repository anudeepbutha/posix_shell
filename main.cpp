#include <bits/stdc++.h>
#include <fcntl.h>
#include <iostream>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "autocomplete.h"
#include "changedir.h"
#include "foreback.h"
#include "history.h"
#include "pinfo.h"
#include "search.h"
#include "signal_handle.h"
#include "showlist.h"
#include "pipeline.h"

using namespace std;

int childpid = -1;

char** command_suggestion(const char* text, int start, int end) {
    if (start == 0)
        return rl_completion_matches(text, command_generator);
    return NULL;
}

void execute_command(vector<string>& words, char* home_path, char* current_path);
vector<string> tokenize(char* str, const char* delim);

int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler);
    
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
        
        if (line_read == NULL) { 
            cout << endl;
            write_history(histfile.c_str());
            free(line_read);
            break;
        }
        
        if (*line_read) add_history(line_read);

        vector<string> commands = tokenize(line_read, ";");
        
        for (auto& cmd_str : commands) {
            char* cmd_cstr = const_cast<char*>(cmd_str.c_str());

            vector<string> pipe_parts = tokenize(cmd_cstr, "|");
            
            if (pipe_parts.size() > 1) {
                vector<vector<string>> pipeline_cmds;
                
                for (auto& part : pipe_parts) {
                    vector<string> words = tokenize(const_cast<char*>(part.c_str()), " \t");
                    if (words.size() > 0) {
                        pipeline_cmds.push_back(words);
                    }
                }
                
                if (pipeline_cmds.size() > 0) {
                    execute_pipeline(pipeline_cmds);
                }
            } else {
                vector<string> words = tokenize(cmd_cstr, " \t");
                if (words.size() > 0) {
                    execute_command(words, home_path, current_path);
                }
            }
        }

        if (history_length > 20)
            remove_history(0);
        free(line_read);
    }
    
    write_history(histfile.c_str());
    return 0;
}

vector<string> tokenize(char* str, const char* delim) {
    vector<string> tokens;
    char* token = strtok(str, delim);
    while (token != NULL) {
        string trimmed = token;
        size_t start = trimmed.find_first_not_of(" \t\n\r");
        size_t end = trimmed.find_last_not_of(" \t\n\r");
        if (start != string::npos && end != string::npos) {
            trimmed = trimmed.substr(start, end - start + 1);
            if (!trimmed.empty())
                tokens.push_back(trimmed);
        }
        token = strtok(NULL, delim);
    }
    return tokens;
}

void execute_command(vector<string>& words, char* home_path, char* current_path) {
    if (words.size() == 0) return;
    
    string infile, outfile;
    int out_red = 0, in_red = 0, append = 0;
    int outfd_dup = -1, infd_dup = -1;
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i] == ">") {
            if (words.size() <= i + 1) {
                cout << "Invalid Arguments" << endl;
                return;
            }
            outfile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            out_red = 1;
            i -= 1;
            append = 0;
        } else if (words[i] == ">>") {
            if (words.size() <= i + 1) {
                cout << "Invalid Arguments" << endl;
                return;
            }
            outfile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            out_red = 1;
            i -= 1;
            append = 1;
        } else if (words[i] == "<") {
            if (words.size() <= i + 1) {
                cout << "Invalid Arguments" << endl;
                return;
            }
            infile = words[i + 1];
            words.erase(words.begin() + i, words.begin() + i + 2);
            in_red = 1;
            i -= 1;
        }
    }
    if (out_red == 1) {
        int flags;
        if (append == 1)
            flags = O_CREAT | O_WRONLY | O_APPEND;
        else
            flags = O_CREAT | O_WRONLY | O_TRUNC;

        int outfd = open(outfile.c_str(), flags, 0644);
        if (outfd < 0) {
            perror("Unable to open output file");
            return;
        } else {
            outfd_dup = dup(STDOUT_FILENO);
            dup2(outfd, STDOUT_FILENO);
            close(outfd);
        }
    }
    if (in_red == 1) {
        int infd = open(infile.c_str(), O_RDONLY);
        if (infd < 0) {
            perror("Unable to open input file");
            if (out_red == 1) {
                dup2(outfd_dup, STDOUT_FILENO);
                close(outfd_dup);
            }
            return;
        } else {
            infd_dup = dup(STDIN_FILENO);
            dup2(infd, STDIN_FILENO);
            close(infd);
        }
    }

    if (words.size() < 1) {
        if (out_red == 1) {
            dup2(outfd_dup, STDOUT_FILENO);
            close(outfd_dup);
        }
        if (in_red == 1) {
            dup2(infd_dup, STDIN_FILENO);
            close(infd_dup);
        }
        return;
    }
    
    if (words[0] == "cd")
        change_directory(words, home_path, current_path);
    else if (words[0] == "echo")
        echo_print(words);
    else if (words[0] == "pwd" && words.size() == 1)
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
    } else
        foreback(words);
    if (out_red == 1) {
        dup2(outfd_dup, STDOUT_FILENO);
        close(outfd_dup);
    }

    if (in_red == 1) {
        dup2(infd_dup, STDIN_FILENO);
        close(infd_dup);
    }
}