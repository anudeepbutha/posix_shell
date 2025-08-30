#ifndef SHOWLIST_h
#define SHOWLIST_h

#include <dirent.h>
#include <grp.h>
#include <iomanip>
#include <iostream>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

string getfilePermissions(mode_t octalcode);

void showlist (vector<string> words) {
    string directory;
    int flag1 = 0, flag2 = 0;
    if (words.size() == 1)
        directory = ".";
    else if (words[1] == ".")
        directory = ".";
    else if (words[1] == "-a") {
        flag1 = 1;
        directory = ".";
    }
    else if (words[1] == "..")
        directory = "..";
    else if (words[1] == "~") {
        passwd *userdetails = getpwuid(getuid());
        directory = userdetails->pw_dir;
    }
    else if (words[1] == "-l" || words[1] == "-la" || words[1] == "-al") {
        directory = ".";
        flag2 = 1;
    }

    DIR *dirstream = opendir(directory.c_str());
    if (dirstream == nullptr) {
        perror("Unable to open the Directory");
        return ;
    }

    struct dirent* drt;
    vector<dirent> filenames;
    while(drt = readdir(dirstream))
        filenames.push_back(*drt);

    if (flag2 == 0) {
        for (int i=0; i<filenames.size(); i++) {
            string str = filenames[i].d_name;
            if (flag1 == 0 && (str == "." || str == "..")) continue;
            cout << str << "  ";
        }
        cout << endl;
    }

    if (flag2 == 1) {
    for (int i=0; i<filenames.size(); i++) {
        string name = filenames[i].d_name;
        if (flag1 == 0 && (name == "." || name == "..")) continue;
        struct stat *statbuffer;
        int result = stat(name.c_str(), statbuffer);
        string permssion = getfilePermissions(statbuffer->st_mode);
        nlink_t hard_links = statbuffer->st_nlink;
        passwd *userdetails = getpwuid(statbuffer->st_uid);
        string user = userdetails->pw_name;
        group *groupdetails = getgrgid(statbuffer->st_gid);
        string group = groupdetails->gr_name;
        off_t size = statbuffer->st_size;
        char time_buf[80];
        // strftime(time_buf, sizeof(time_buf), "%b %d %H:%M"); // need to complete time
        cout << permssion << " " << hard_links << " " << user << " " << group << " " << setw(10) << size 
        << " " << name << endl;
    }
}
    closedir(dirstream);
}

string getfilePermissions(mode_t octalcode) {
    char octpermissions[10];
    octpermissions[0] = (S_ISDIR(octalcode) ? 'd' : '-');
    octpermissions[1] = ((octalcode & S_IRUSR) ? 'r' : '-');
    octpermissions[2] = ((octalcode & S_IWUSR) ? 'w' : '-');
    octpermissions[3] = ((octalcode & S_IXUSR) ? 'x' : '-');
    octpermissions[4] = ((octalcode & S_IRGRP) ? 'r' : '-');
    octpermissions[5] = ((octalcode & S_IWGRP) ? 'w' : '-');
    octpermissions[6] = ((octalcode & S_IXGRP) ? 'x' : '-');
    octpermissions[7] = ((octalcode & S_IROTH) ? 'r' : '-');
    octpermissions[8] = ((octalcode & S_IWOTH) ? 'w' : '-');
    octpermissions[9] = ((octalcode & S_IXOTH) ? 'x' : '-');
    return octpermissions;
}

#endif