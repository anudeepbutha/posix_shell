#ifndef HISTORY_h
#define HISTORY_h

#include <iostream>
#include <readline/history.h>
#include <vector>

using namespace std;

void print_history(vector<string> words) {
    HIST_ENTRY **hist_list = history_list();
    int start = 0;
    if (words.size() == 2){
        if (history_length > stoi(words[1]))
            start = history_length - stoi(words[1]);
        for (int i = start; hist_list[i]; i++)
            cout << hist_list[i]->line << endl;
        return ;
    }
    if (history_length > 20)
        start = history_length - 20;
    for (int i = start; hist_list[i]; i++)
        cout << hist_list[i]->line << endl;
}

#endif