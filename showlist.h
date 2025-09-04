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
#include <stdio.h>

using namespace std;

string getfilePermissions(mode_t octalcode);

void showlist (vector<string> words) {
    string directory = ".";
    int flaga = 0, flagl = 0;

    for (size_t i=1; i<words.size(); i++) {
        if (words[i] == "-a")
            flaga = 1;
        else if (words[i] == "-l")
            flagl = 1;
        else if (words[i] == "-al" || words[i] == "-la") {
            flaga = 1; flagl = 1;
        }
        else if (words[i] == "~") {
            passwd *userdetails = getpwuid(getuid());
            directory = userdetails->pw_dir;
        }
        else
            directory = words[i];
    }

    struct stat st;
    if (stat(directory.c_str(), &st) == -1) {
        perror("ls");
        return;
    }

    if (S_ISREG(st.st_mode) && flagl == 0) {
        cout << directory << endl;
        return;
    }

    DIR *dirstream = opendir(directory.c_str());
    if (dirstream == nullptr) {
        perror("Unable to open the Directory");
        return ;
    }

    struct dirent* drt;
    vector<dirent> filenames;
    while((drt = readdir(dirstream)))
        filenames.push_back(*drt);

    if (flagl == 0) {
        for (size_t i=0; i<filenames.size(); i++) {
            string str = filenames[i].d_name;
            if (flaga == 0 && (str == "." || str == "..")) continue;
            cout << str << "  ";
        }
        cout << endl;
    }

    if (flagl == 1) {
    for (size_t i=0; i<filenames.size(); i++) {
        string name = filenames[i].d_name;
        if (flaga == 0 && (name == "." || name == "..")) continue;
        struct stat statbuffer;
        int result = stat(name.c_str(), &statbuffer);
        if (result != 0) continue;

        string permission = getfilePermissions(statbuffer.st_mode);
        nlink_t hard_links = statbuffer.st_nlink;
        passwd *userdetails = getpwuid(statbuffer.st_uid);
        string user = userdetails->pw_name;
        group *groupdetails = getgrgid(statbuffer.st_gid);
        string group = groupdetails->gr_name;
        off_t size = statbuffer.st_size;
        char timebuf[64];
        struct tm lt;
        localtime_r(&statbuffer.st_mtime, &lt);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", &lt);
        printf("%s %ld %10s %10s %10ld %13s %s\n", permission.c_str(), hard_links, user.c_str(), 
        group.c_str(), size, timebuf, name.c_str());
    }
}
    closedir(dirstream);
}

string getfilePermissions(mode_t octalcode) {
    char octpermissions[11];
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
    octpermissions[10] = '\0';
    return octpermissions;
}

#endif