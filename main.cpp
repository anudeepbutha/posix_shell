#include <bits/stdc++.h>
#include <fcntl.h>
#include <iostream>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "changedir.h"
#include "foreback.h"
#include "pinfo.h"
#include "search.h"
#include "showlist.h"

using namespace std;

void signalHandler (int signum) {
    cout << endl << "Hello";
}

int main () {
    // signal(SIGINT, signalHandler);
    // signal(SIGINT, signalHandler);
    // signal(SIGINT, signalHandler);
    passwd *userdetails = getpwuid(getuid());
    char* username = userdetails->pw_name;

    char hostname[1024];
    gethostname(hostname, sizeof(hostname));

    char home_path[1024];
    getcwd(home_path, sizeof(home_path));

    while (1)
    {
        char current_path[1024];
        getcwd(current_path, sizeof(current_path));
        static char *line_read = (char *)NULL;

        string prompt;
        if (strcmp(current_path, home_path) == 0)
            prompt = string(username) + "@" + string(hostname) + ":~>";
        else
            prompt = string(username) + "@" + string(hostname) + ":" + string(current_path) + ">";

        line_read = readline(prompt.c_str());
        if (line_read == NULL) continue;
        
        add_history(line_read);
        vector <string> words;
        char* token = strtok(line_read, " ");
        while (token != NULL) {
            words.push_back(token);
            token = strtok(nullptr, " ");
        }

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
        else if (words[0] == "-1")
            raise(SIGINT);
        else
            foreback(words);
        free(line_read);
    }
    return 0;
}

// int out_red= 0, in_red = 0;
//         int outfd_dup = -1, infd_dup = -1;
//         for (int i=0; i<words.size(); i++) {
//             string filename;
//             if (words[i] == ">") {
//                 if (words.size() <= i+1) {
//                     cout << "Invalid Arguments" << endl;
//                     continue;
//                 }
//                 filename = words[i+1];
//                 int openfd = open(filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
//                 if (openfd < 0) {
//                     cout << "unable to open the file" << endl;
//                     continue;
//                 }
//                 int fdcopy = dup(openfd);
//                 words.erase(words.begin()+i, words.begin()+i+2);
//                 out_red = 1; i-=1;
//                 outfd_dup = dup(STDOUT_FILENO);
//             }
//             else if (words[i] == ">>") {
//                 if (words.size() <= i+1) {
//                     cout << "Invalid Arguments" << endl;
//                     continue;
//                 }
//                 filename = words[i+1];
//                 int openfd = open(filename.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644);
//                 if (openfd < 0) {
//                     cout << "unable to open the file" << endl;
//                     continue;
//                 }
//                 int fdcopy = dup(openfd);
//                 words.erase(words.begin()+i, words.begin()+i+2);
//                 out_red = 1; i-=1;
//                 outfd_dup = dup(STDOUT_FILENO);
//             }
//             else if (words[i] == "<") {
//                 if (words.size() <= i+1) {
//                     cout << "Invalid Arguments" << endl;
//                     continue;
//                 }
//                 filename = words[i+1];
//                 int openfd = open(filename.c_str(), O_RDONLY);
//                 if (openfd < 0) {
//                     cout << "unable to open the file" << endl;
//                     continue;
//                 }
//                 int fdcopy = dup(openfd);
//                 words.erase(words.begin()+i, words.begin()+i+2);
//                 in_red = 1; i-=1;
//                 infd_dup = dup(STDIN_FILENO);
//             }
//         }