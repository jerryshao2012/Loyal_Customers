//
// Created by Jerry Shao on 2023-12-14.
//

#include "string_helper.h"

/**
 * Generate a string with sentence_size in length
 * @param sentence_size string's length
 * @return a string with sentence_size in length
 */
string string_helper::generate_random_string(const int sentence_size) {
    string result;
    random_device rd;
    for (int i = 0; i < sentence_size; i++) {
        result += char('a' + rd() % 26);
    }
    return result;
}

/**
 * Split a string using delimiter
 * @param s string to split
 * @param delimiter delimiter to split string
 * @return split string using delimiter into a vector of string
 */
vector<string> string_helper::split(const string &s, const string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

/**
 * Join a vector of string using delimiter
 * @param v a vector of string
 * @param delimiter delimiter to join vector
 * @return a string join a vector of string using delimiter
 */
string string_helper::join(const vector<string> &v, const string &delimiter) {
    string res;
    if (auto i = v.begin(), e = v.end(); i != e) {
        res += *i++;
        for (; i != e; ++i) res.append(delimiter).append(*i);
    }
    return res;
}

/**
 * Search str and replace string from with to
 * @param str string to search
 * @param from replace from string
 * @param to replace to string
 * @return replaced string
 */
bool string_helper::replace(string &str, const string &from, const string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

/**
 * Generate a new file name with string_before_extension
 * @param file_name file name
 * @param string_before_extension string before extension
 * @return generated a new file name with string_before_extension
 */
string string_helper::get_new_file_name(const string &file_name, const string &string_before_extension) {
    const size_t last_dot = file_name.find_last_of('.');
    string return_file_name = file_name;
    if (last_dot != string::npos) {
        return_file_name = file_name.substr(0, last_dot);
        return_file_name.append(string_before_extension);
        const string extension = file_name.substr(last_dot);
        return_file_name.append(extension);
    } else
        return_file_name.append(string_before_extension);
    return return_file_name;
}

/**
 * uuid v4 helper (not for production)
 */
static random_device rd;
static mt19937 gen(rd());
static uniform_int_distribution<> dis(0, 15);
static uniform_int_distribution<> dis2(8, 11);

string string_helper::generate_uuid_v4() {
    stringstream ss;
    int i;
    ss << hex;
    for (i = 0; i < 8; i++)
        ss << dis(gen);
    ss << "-";
    for (i = 0; i < 4; i++)
        ss << dis(gen);
    ss << "-4";
    for (i = 0; i < 3; i++)
        ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++)
        ss << dis(gen);

    ss << "-";
    for (i = 0; i < 12; i++)
        ss << dis(gen);
    return ss.str();
}
