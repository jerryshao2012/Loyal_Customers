//
// Created by Jerry Shao on 2023-12-11.
//

#include <iostream>
#include <fstream>
#include <string>
#include "map"
#include "set"

#include "string_helper.h"

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
 *
 * map: Maps are associative containers that store elements formed by a combination of a key value and a mapped value,
 * following a specific order.
 * https://cplusplus.com/reference/map/map/
 */

// Result for loyal customers
set<string> loyal_customers;

void find_loyal_customers(map<string, map<int, set<string>>> &pages_visited_by_customer,
                          const string &process_log_file_name,
                          const int day) {
    ifstream process_log_file_reader;
    process_log_file_reader.open(process_log_file_name);
    string line;
    while (getline(process_log_file_reader, line)) {
        const vector<string> data = string_helper::split(line, ",");
        const auto page_id = data[1];       // NOLINT(*-unnecessary-copy-initialization)
        const auto customer_id = data[2];   // NOLINT(*-unnecessary-copy-initialization)
        if (loyal_customers.count(customer_id))
            continue;
        map<int, set<string>> page_ids_by_day;
        if (pages_visited_by_customer.count(customer_id)) {
            page_ids_by_day = pages_visited_by_customer.at(customer_id);

            for (auto each_day = page_ids_by_day.begin(); each_day != page_ids_by_day.end(); each_day++) {
                if (each_day->first == day) {
                    // Same day: store the page id visited
                    set<string> page_ids = each_day->second;
                    page_ids.insert(page_id);
                } else {
                    // Two different days
                    if (    // More than two pages for different days
                            page_ids_by_day.size() >= 2
                            // Just different pages for two days
                            || *(each_day->second.begin()) != page_id) {
                        // Add into loyal customers
                        loyal_customers.insert(customer_id);
                        // Remove from processing customer list
                        pages_visited_by_customer.erase(customer_id);
                        break;
                    }
                }
            }
        } else {
            set<string> page_ids;
            page_ids.insert(page_id);
            page_ids_by_day.insert({day, page_ids});
            pages_visited_by_customer.insert({customer_id, page_ids_by_day});
        }
    }
    process_log_file_reader.close();
}

int main() {
    const string path = "../logs";
    // Store pages visited by customer: the key is customer id and the value is set of page ids by day
    map<string, map<int, set<string>>> pages_visited_by_customer;

    find_loyal_customers(pages_visited_by_customer, path + "/day1.log", 1);
    find_loyal_customers(pages_visited_by_customer, path + "/day2.log", 2);
    find_loyal_customers(pages_visited_by_customer, path + "/day3.log", 3);

    cout << "There are " << loyal_customers.size() << " loyal customers" << endl;
    for (const auto &customer_id: loyal_customers) cout << customer_id << endl;

    return 0;
}
