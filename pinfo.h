#ifndef PINFO_H
#define PINFO_h

#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;

void process_info(vector<string> words) {
    int pid;
    if (words.size() == 1)
        pid = getpid();
    else if (words.size() == 2)
        pid = stoi(words[1]);
    else {
        cout << "Invalid Agruments" << endl;
        return ;
    }

    char buffer[1024];
    char pstatus;
    string vmsize;
    string status_path = "/proc/" + to_string(pid) + "/status";
    string stat_path = "/proc/" + to_string(pid) + "/stat";

    FILE *fp = fopen(status_path.c_str(), "r");
    if (fp == NULL) {
        cout << "No such Process exists" << endl;
        return ;
    }

    if (fp) {
        while(fgets(buffer, size(buffer), fp)) {
            int flag1 = 0, flag2 = 0;
            if (strncmp(buffer, "State:", 6) == 0 && flag1 == 0) {
                pstatus = buffer[7];
                flag1 =1;
                continue;
            }
            else if (strncmp(buffer, "VmSize:", 7) == 0 && flag2 == 0) {
                vmsize = buffer+8;
                flag2 =1;
                continue;
            }
            if (flag1 == 1 && flag2 == 1)
                break;
        }
    }

    fclose(fp);

    FILE *fp2 = fopen(stat_path.c_str(), "r");
    if (fp2 == NULL) {
        cout << "No such Process exists" << endl;
        return ;
    }

    int pgrp, tpgid;
    fscanf(fp2, "%*d %*s %*c %*d %d %*d %*d %d", &pgrp, &tpgid);
    fclose(fp2);

    bool is_fg;
    if (pgrp == tpgid)
        is_fg = true;
    else
        is_fg = false;

    string exeporo = "/proc/" + to_string(pid) + "/exe";
    char exec_path[128];
    ssize_t path_length = readlink(exeporo.c_str(), exec_path, 127);
    if (path_length == -1) {
        perror ("Unable read the exe path");
        return ;
    }
    exec_path[path_length] = '\0';
    cout << "Process Status -- " << pstatus << (is_fg ? "+" : "") << endl;
    cout << "memory -- " << vmsize;
    cout << "Executable Path -- " << exec_path << endl;
}

#endif