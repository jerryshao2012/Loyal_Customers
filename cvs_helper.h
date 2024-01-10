//
// Created by Jerry Shao on 2023-12-14.
//

#ifndef TEST_CVS_HELPER_H
#define TEST_CVS_HELPER_H

#include <string>

using namespace std;

class cvs_helper {
public:
    static vector<vector<string>> read_cvs(const string &file_name);

    static void write_cvs(const string &file_name, const vector<vector<string>> &lines);
};

#endif //TEST_CVS_HELPER_H
