//
// Created by Jerry Shao on 2023-12-14.
//

#ifndef TEST_STRING_HELPER_H
#define TEST_STRING_HELPER_H

#include <string>
#include <iostream>
#include <random>
#include <sstream>

using namespace std;

class string_helper {
public:
    /**
     * Generate a string with sentence_size in length
     * @param sentence_size string's length
     * @return a string with sentence_size in length
     */
    static string generate_random_string(int sentence_size);

    /**
     * Split a string using delimiter
     * @param s string to split
     * @param delimiter delimiter to split string
     * @return split string using delimiter into a vector of string
     */
    static vector<string> split(const string &s, const string &delimiter);

    /**
     * Join a vector of string using delimiter
     * @param v a vector of string
     * @param delimiter delimiter to join vector
     * @return a string join a vector of string using delimiter
     */
    static string join(const vector<string> &v, const string &delimiter);

    /**
     * Search str and replace string from with to
     * @param str string to search
     * @param from replace from string
     * @param to replace to string
     * @return replaced string
     */
    static bool replace(string &str, const string &from, const string &to);

    /**
     * Generate a new file name with string_before_extension
     * @param file_name file name
     * @param string_before_extension string before extension
     * @return generated a new file name with string_before_extension
     */
    static string get_new_file_name(const string &file_name, const string &string_before_extension);

    /**
     * uuid v4 helper (not for production)
     */
    static string generate_uuid_v4();

};

#endif //TEST_STRING_HELPER_H
