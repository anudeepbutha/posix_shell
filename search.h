#ifndef SEARCH_H
#define SEARCH_H

#include <dirent.h>
#include <iostream>
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

bool recursive_search (string directory, string name) {
    DIR *dirstream = opendir(directory.c_str());
    struct dirent* drt;
    while(drt = readdir(dirstream)) {
        if (drt->d_name == name) 
            return true;
        if (drt->d_name == "." || drt->d_name == "..")
            continue;
        if (S_ISDIR(drt->d_type) == 1)
        {
            string dirpath = "./" + string(drt->d_name);
            return recursive_search(dirpath, name);
        }
    }
    return false;
}

#endif