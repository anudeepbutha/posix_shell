#include <bits/stdc++.h>
#include <iostream>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#include "changedir.h"
#include "foreback.h"
#include "pinfo.h"
#include "search.h"
#include "showlist.h"

using namespace std;

int main () {
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
        else if(words[0] == "gedit")
            foreback(words);
        else if (words[0] == "pinfo")
            process_info(words);
        else if (words[0] == "search")
            search(words);

        free(line_read);
    }
    return 0;
}

        // stringstream ss(line_read);
        // string s;
        // vector<string> words;
        // while (getline(ss, s, ' ')) 
        //     words.push_back(s);
        // if (words.size() == 0) continue;