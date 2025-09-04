#ifndef SEARCH_H
#define SEARCH_H

#include <dirent.h>
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

using namespace std;

bool recursive_search (string directory, string name);

void search (vector<string> words) {
    if (words.size() != 2) {
        cout << "Invalid arguments" << endl;
        return ;
    }

    string fname = words[1];
    bool result = recursive_search(".", fname);

    if (result == 1)
        cout << "True" << endl;
    else
        cout << "False" << endl;
}

bool recursive_search (string directory, string fname) {
    DIR *dirstream = opendir(directory.c_str());
    if(dirstream == NULL)
        return false;

    struct dirent* drt;
    while((drt = readdir(dirstream)) != NULL) {
        if (fname == drt->d_name) {
            closedir(dirstream);
            return true;
        }

        if (strcmp(drt->d_name, ".") == 0 || strcmp(drt->d_name, "..") == 0)
            continue;

        string absPath = directory + "/" + drt->d_name;
        struct stat st;
        if (stat(absPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            if (recursive_search(absPath, fname)) {
                closedir(dirstream);
                return true;
            }
        }
    }
    closedir(dirstream);
    return false;
}

#endif