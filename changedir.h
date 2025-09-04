#ifndef CHANGEDIR_H
#define CHANGEDIR_H

#include <cstring>
#include <iostream>
#include <pwd.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

string split(char* buffer);

string old_path;
int isFirst = 1;

void change_directory (vector<string> words, char* home_path, char* current_path) {
    int result = 0;
    if (words.size() == 1)
        result = chdir(home_path);
    else if (words[1] == "~") {
        passwd *userdetails = getpwuid(getuid());
        char* path = userdetails->pw_dir;
        result = chdir(path);
    }
    else if (words[1] == "-") {
        if (isFirst == 1) {
            cout << "Previous Directory Not Set" << endl;
            return ;
        }
        cout << old_path << endl;
        result = chdir(old_path.c_str());
    }
    else if (words.size() == 2) {
        string new_path = current_path;
        new_path.append("/");
        new_path.append(words[1]);
        result = chdir(new_path.c_str());
    }
    else
        cout << "Invalid Arguments" << endl;
    if (result == -1)
        perror("Invalid Arguments");
    else {
        old_path = current_path;
        isFirst = 0;
    }
}

void echo_print(vector<string> words) {
    for (size_t i=1; i<words.size(); i++)
        cout << words[i] << " ";
    cout << endl;
}

string split(char* buffer) {
    vector <string> path;
    char* token = strtok(buffer, "/");
    while (token != NULL) {
        path.push_back(token);
        token = strtok(nullptr, "/");
    }
    string previous_path;
    for(size_t i=0; i<path.size()-1; i++) {
        previous_path.append("/");
        previous_path.append(path[i]);
    }
    return previous_path;
}

#endif