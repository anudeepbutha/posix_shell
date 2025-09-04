#ifndef AUTOCOMPLETE_h
#define AUTOCOMPLETE_h

#include <dirent.h>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <vector>

using namespace std;

vector<string> env_paths = {
    "/bin",
    "/usr/bin",
    "/usr/local/bin"
};

vector<string> getSuggestion() {
    vector<string> suggestion;

    for (size_t i=0; i<env_paths.size(); i++) {
        DIR *dirstream = opendir(env_paths[i].c_str());
        if (dirstream == NULL) continue;

        struct dirent* drt;
        while ((drt = readdir(dirstream)) != NULL) {
            struct stat st;
            string abspath = env_paths[i]+ "/" + drt->d_name;

            if (stat(abspath.c_str(), &st) == 0 && st.st_mode & S_IXUSR)
                suggestion.push_back(drt->d_name);
        }
        closedir(dirstream);
    }
    return suggestion;
}

char* command_generator(const char* text, int state) {
    static vector<string> matches;
    static size_t list_index;

    if (state == 0) {
        matches.clear();
        list_index = 0;

        vector<string> pathExecs = getSuggestion();
        for (string str : pathExecs)
            if (strncmp(str.c_str(), text, strlen(text)) == 0)
                matches.push_back(str);
    }

    if (list_index < matches.size())
        return strdup(matches[list_index++].c_str());

    return NULL;
}

#endif