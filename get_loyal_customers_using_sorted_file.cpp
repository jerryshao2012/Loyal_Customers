//
// Created by Jerry Shao on 2023-12-11.
//

#include <iostream>
#include <fstream>
#include <string>
#include "map"
#include "set"

#include "string_helper.h"
#include "cvs_helper.h"

using namespace std;

/**
 * Let’s say we have a website and we keep track of what pages customers are viewing, for things like business metrics.
 *
 * Every time somebody comes to the website, we write a record to a log file consisting of Timestamp, PageId,
 * CustomerId. At the end of the day we have a big log file with many entries in that format. And for every day
 * we have a new file.
 *
 * Now, given two log files (log file from day 1 and log file from day 2) we want to generate a list of
 * ‘loyal customers’ that meet the criteria of: (a) they came on both days, and (b) they visited at least two unique
 * pages.
 *
 * vector: Vectors are sequence containers representing arrays that can change in size.
 * https://cplusplus.com/reference/vector/vector/
 *
 * set: Sets are containers that store unique elements following a specific order.
 * https://cplusplus.com/reference/set/set/
 */

// Result for loyal customers
set<string> loyal_customers;

void sort_log_file(const string &file_name, const vector<int> &sort_array) {
    vector<vector<string>> lines = cvs_helper::read_cvs(file_name);
    sort(lines.begin(), lines.end(),
         [sort_array](const vector<string> &row1, const vector<string> &row2) {
             for (auto &sort_column: sort_array) {
                 if (sort_column >= 0 && sort_column < row1.size() && sort_column < row2.size() &&
                     row1.at(sort_column) != row2.at(sort_column))
                     return row1.at(sort_column) < row2.at(sort_column);
             }
             return false;
         });
    const string sorted_file_name
            = string_helper::get_new_file_name(file_name, "_sorted");
    cvs_helper::write_cvs(sorted_file_name, lines);
}

void find_loyal_customers(const string &day0_log_file_name, const string &process_log_file_name) {
    const ifstream day0_log_file_exists(day0_log_file_name.c_str());
    if (day0_log_file_exists.good()) {
        const string temp_log_file_name
                = string_helper::get_new_file_name(process_log_file_name, "_temp");
        // Remove file
        filesystem::remove(temp_log_file_name.c_str());
        ofstream temp_log_file_writer;
        temp_log_file_writer.open(temp_log_file_name);

        ifstream day_log_file_reader;
        day_log_file_reader.open(process_log_file_name);
        string day_log_file_line;
        getline(day_log_file_reader, day_log_file_line);

        ifstream day0_log_file_reader;
        day0_log_file_reader.open(day0_log_file_name);
        string day0_log_file_line;
        getline(day0_log_file_reader, day0_log_file_line);

        while (!day_log_file_reader.eof() && !day0_log_file_reader.eof()) {
            const vector<string> day_log_data = string_helper::split(day_log_file_line, ",");
            const auto &page_id = day_log_data[1];
            const auto &customer_id = day_log_data[2];
            if (loyal_customers.count(customer_id)) {
                getline(day_log_file_reader, day_log_file_line);
                continue;
            }
            while (!day0_log_file_reader.eof()) {
                const vector<string> day0_log_data = string_helper::split(day0_log_file_line, ",");
                const auto &page_id0 = day0_log_data[1];
                const auto &customer_id0 = day0_log_data[2];
                if (loyal_customers.count(customer_id0)) {
                    getline(day0_log_file_reader, day0_log_file_line);
                    continue;
                }
                if (strcmp(customer_id0.c_str(), customer_id.c_str()) > 0) {
                    // Reserve for next day 0 log
                    temp_log_file_writer << string_helper::join(day_log_data, ",") << endl;
                    getline(day_log_file_reader, day_log_file_line);
                    break;
                } else if (strcmp(customer_id0.c_str(), customer_id.c_str()) < 0) {
                    // Reserve for next day 0 log
                    temp_log_file_writer << string_helper::join(day0_log_data, ",") << endl;
                    getline(day0_log_file_reader, day0_log_file_line);
                } else {
                    // Same customer in different to days
                    if (page_id0 != page_id) {
                        // Match two unique pages
                        loyal_customers.insert(customer_id);

                        getline(day0_log_file_reader, day0_log_file_line);
                        while (!day0_log_file_reader.eof()) {
                            const vector<string> pre_day0_log_data
                                    = string_helper::split(day0_log_file_line, ",");
                            const auto &pre_customer_id0 = pre_day0_log_data[2];
                            if (pre_customer_id0 != customer_id)
                                break;
                            getline(day0_log_file_reader, day0_log_file_line);
                        }

                        getline(day_log_file_reader, day_log_file_line);
                        while (!day_log_file_reader.eof()) {
                            const vector<string> pre_day_log_data
                                    = string_helper::split(day_log_file_line, ",");
                            const auto &pre_customer_id = pre_day_log_data[2];
                            if (pre_customer_id != customer_id)
                                break;
                            getline(day_log_file_reader, day_log_file_line);
                        }
                    } else {
                        // Same page id for between two days
                        getline(day0_log_file_reader, day0_log_file_line);
                        if (!day0_log_file_reader.eof()) {
                            const vector<string> pre_day0_log_data
                                    = string_helper::split(day0_log_file_line, ",");
                            const auto &pre_customer_Id0 = pre_day0_log_data[2];
                            if (pre_customer_Id0 == customer_id) continue;
                        }

                        getline(day_log_file_reader, day_log_file_line);
                    }
                    break;
                }
            }
        }

        while (!day_log_file_reader.eof()) {
            temp_log_file_writer << day_log_file_line << endl;
            getline(day_log_file_reader, day_log_file_line);
        }
        day_log_file_reader.close();
        while (!day0_log_file_reader.eof()) {
            temp_log_file_writer << day0_log_file_line << endl;
            getline(day0_log_file_reader, day0_log_file_line);
        }
        day0_log_file_reader.close();

        // For next day 0 log file
        temp_log_file_writer.close();
        filesystem::copy(temp_log_file_name, day0_log_file_name);
        // Remove file
        filesystem::remove(temp_log_file_name.c_str());
    } else {
        filesystem::copy(process_log_file_name, day0_log_file_name);
    }
}

int main() {
    const string path = "../logs";

    // sort index for cvs column default to asc
    const vector<int> sort_array = {2, 1};

    sort_log_file(path + "/day1.log", sort_array);
    sort_log_file(path + "/day2.log", sort_array);
    sort_log_file(path + "/day3.log", sort_array);

    const string day0_log_file_name = path + "/day0_sorted.log";
    // Remove file
    filesystem::remove(day0_log_file_name.c_str());

    find_loyal_customers(day0_log_file_name, path + "/day1_sorted.log");
    find_loyal_customers(day0_log_file_name, path + "/day2_sorted.log");
    find_loyal_customers(day0_log_file_name, path + "/day3_sorted.log");

    // Clean up: remove file
    filesystem::remove(day0_log_file_name.c_str());

    cout << "There are " << loyal_customers.size() << " loyal customers" << endl;
    for (const auto &customer_id: loyal_customers) cout << customer_id << endl;

    return 0;
}
