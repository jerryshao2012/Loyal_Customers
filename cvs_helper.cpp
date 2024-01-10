//
// Created by Jerry Shao on 2023-12-14.
//

#include <fstream>
#include "cvs_helper.h"
#include "string_helper.h"

vector<vector<string>> cvs_helper::read_cvs(const string &file_name) {
    vector<vector<string>> lines;
    ifstream file_reader;
    file_reader.open(file_name);
    string line;
    while (getline(file_reader, line)) {
        const vector<string> data = string_helper::split(line, ",");
        lines.push_back(data);
    }
    file_reader.close();
    return lines;
}

void cvs_helper::write_cvs(const string &file_name, const vector<vector<string>> &lines) {
    // Remove file
    remove(file_name.c_str());

    ofstream file_writer;
    file_writer.open(file_name);
    if (file_writer.is_open())
        for (const auto &line: lines) {
            file_writer << string_helper::join(line, ",") << endl;
        }
    file_writer.close();
}