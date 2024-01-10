//
// Created by Jerry Shao on 2023-12-11.
//
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <queue>
#include <random>

#include "string_helper.h"

using namespace std;

/**
 *
 * This C++ implementation serves as a demonstration of the External Sort algorithm.
 *
 * External sorting becomes necessary when the data to be sorted exceeds the capacity of the system's RAM, requiring a
 * method to access and process it in manageable blocks. The process involves two primary steps: first, reading data in
 * blocks into memory, sorting it, and writing it to temporary files; second, merging these temporary files, typically
 * employing a technique akin to merge sort, combining two, three, four, or more files at a time.
 *
 * One prominent instance of external sorting is the external merge sort algorithm, utilizing a K-way merge approach.
 * It sorts chunks of data that individually fit into the available RAM, subsequently merging these sorted chunks.
 *
 * The algorithm initially sorts a fixed number, M, of items, storing the sorted lists in external memory. It then
 * recursively performs an M/B-way merge on these lists. During this merge, B elements from each sorted list are loaded
 * into the internal memory, and the minimum is repeatedly extracted.
 *
 * For example, sorting 900 megabytes of data with a RAM constraint of 100 megabytes involves:
 * 1. Reading 100 MB of data into the main memory and sorting it using a conventional method such as quicksort.
 * 2. Storing the sorted data onto disk.
 * 3. Iterating steps 1 and 2 until all 900MB are organized into 100 MB chunks (in this case, 9 chunks), necessitating
 * merging into a single output file.
 * 4. Reading the initial 10 MB (calculated as 100MB / (9 chunks + 1)) from each sorted chunk into input buffers in the
 * main memory, reserving 10 MB for an output buffer. In practical scenarios, adjusting buffer sizes may enhance
 * performance.
 * 5. Executing a 9-way merge, storing the output in the buffer. Upon buffer saturation, writing it to the final sorted
 * file and clearing it. As each of the 9 input buffers depletes, refilling it with the subsequent 10 MB of its
 * associated 100 MB sorted chunk until all chunk data is processed. This sequential loading of chunk parts as needed
 * is pivotal in the external merge sort's functionality.
 *
 * Historically, instead of sorting, a replacement-selection algorithm[3] was occasionally employed for initial
 * distribution. This approach, on average, produced half the number of output chunks, each twice the length.
 *
 */
static const size_t SIZEOF_CHAR = sizeof(char);

const clock_t begin_time = clock();

void generate_csv_log_file(const string &file_name, const long file_size) {
    // Get start date
    time_t start_time;
    time(&start_time);
    tm *local_time = localtime(&start_time);
    local_time->tm_hour = -1;
    local_time->tm_min = -1;
    local_time->tm_sec = -1;
    start_time = mktime(local_time);
    cout << ctime(&start_time);

    // Get end date
    local_time->tm_mday += 1;
    const time_t end_time = mktime(local_time);
    cout << ctime(&end_time);
    const long period = end_time - start_time;
    // day_time generator
    default_random_engine generator(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<long> day_time_distribution(0, period);

    // Generate page ids & customer ids
    string page_ids[2000];
    string customer_ids[500];
    for (auto &page_id: page_ids)
        page_id = string_helper::generate_random_string(16);
    for (auto &customer_id: customer_ids)
        customer_id = string_helper::generate_uuid_v4();
    // page_id generator and customer_id generator
    uniform_int_distribution<long> page_id_distribution(0, size(page_ids) - 1);
    uniform_int_distribution<long> customer_id_distribution(0, size(customer_ids) - 1);

    // Remove the old file
    filesystem::remove(file_name.c_str());
    // Create output cvs log file
    ofstream output;
    output.open(file_name.c_str());

    // Generate log in cvs format: timestamp, page_id, customer_id
    unsigned long total_size_so_far = 0;
    unsigned long sentence_size;
    while (total_size_so_far < file_size) {
        const long timestamp = start_time + day_time_distribution(generator);
        auto t1 = page_id_distribution(generator);
        const string page_id = page_ids[t1];
        auto t2 = customer_id_distribution(generator);
        const string customer_id = customer_ids[t2];
        cout << t1 << "=" << page_id << "," << t2 << "=" << customer_id << endl;

        stringstream ss;
        ss << timestamp << "," << page_id << "," << customer_id << endl;
        sentence_size = ss.str().size() * SIZEOF_CHAR;
        output << ss.str();
        total_size_so_far += sentence_size;
    }

    output.close();
    cout << "File " << file_name << " has been generated: " << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000
         << " milliseconds." << endl;
}

bool comparator(const vector<int> &sort_array, const vector<string> &row1, const vector<string> &row2) {
    for (auto &sort_column: sort_array)
        if (sort_column >= 0 && sort_column < row1.size() && sort_column < row2.size() &&
            row1.at(sort_column) != row2.at(sort_column))
            return row1.at(sort_column) < row2.at(sort_column);
    return false;
}

struct HeapNode {
    string sentence;
    int index;
    vector<int> sort_array;

    HeapNode(string a, int b, vector<int> c) : sentence(std::move(a)), index(b), sort_array(std::move(c)) {}

    bool operator<(const HeapNode &rhs) const {
        const auto row1 = string_helper::split(rhs.sentence, ",");
        const auto row2 = string_helper::split(sentence, ",");
        return comparator(sort_array, row1, row2);
    }
};

int input_cvs_file(const string &input_csv_file_name, const long total_mem, const vector<int> &sort_array) {
    ifstream input_cvs_file_stream;
    input_cvs_file_stream.open(input_csv_file_name.c_str());

    if (!input_cvs_file_stream.good()) {
        cout << "File '" << input_csv_file_name << "' is not found!" << endl << "Exit program!" << endl;
        exit(-1);
    }

    // Get file size in bytes
    input_cvs_file_stream.seekg(0, ifstream::end);
    const long input_file_size = input_cvs_file_stream.tellg();
    input_cvs_file_stream.seekg(0, ifstream::beg);
    cout << "-------------------------------------------------------" << endl;
    cout << "The size of the file chosen is (in bytes): " << input_file_size << endl;

    int run_count = 0;
    unsigned long total_mem_so_far = 0;

    vector<vector<string>> rows;

    cout << "File " << input_csv_file_name << " is being read!" << endl;
    cout << "-------------------------------------------------------\n\n" << endl;

    cout << "-------------------------------------------------------" << endl;
    while (!input_cvs_file_stream.eof()) {
        string line;
        getline(input_cvs_file_stream, line);
        if (total_mem_so_far + line.size() * SIZEOF_CHAR < total_mem) {
            // Add into rows for sort in memory
            total_mem_so_far += line.size() * SIZEOF_CHAR + 1;
            rows.push_back(string_helper::split(line, ","));
        } else {
            // Sort in memory
            sort(rows.begin(), rows.end(),
                 [&sort_array](vector<string> &row1, vector<string> &row2) {
                     return comparator(sort_array, row1, row2);
                 });

            run_count++;
            stringstream string_stream;
            string_stream << "run_" << run_count << ".csv";
            cout << "Writing " << string_stream.str() << endl;
            ofstream run_cvs_file_output_stream;
            run_cvs_file_output_stream.open(string_stream.str());

            unsigned long data_size = rows.size();
            for (int i = 0; i < data_size - 1; i++)
                run_cvs_file_output_stream << string_helper::join(rows[i], ",") << endl;
            // Last line don't have a new line
            if (data_size > 0)
                run_cvs_file_output_stream << string_helper::join(rows[data_size - 1], ",");
            run_cvs_file_output_stream.close();

            // New run started
            rows.clear();
            total_mem_so_far = line.size() * SIZEOF_CHAR;
            rows.push_back(string_helper::split(line, ","));
        }
    }
    input_cvs_file_stream.close();

    if (!rows.empty()) {
        sort(rows.begin(), rows.end(),
             [sort_array](vector<string> &row1, vector<string> &row2) {
                 return comparator(sort_array, row1, row2);
             });

        run_count++;
        stringstream string_stream;
        string_stream << "run_" << run_count << ".csv";
        cout << "Writing " << string_stream.str() << endl;
        ofstream run_cvs_file_output_stream;
        run_cvs_file_output_stream.open(string_stream.str());

        unsigned long data_size = rows.size();
        for (int i = 0; i < data_size - 1; i++) {
            run_cvs_file_output_stream << string_helper::join(rows[i], ",");
            run_cvs_file_output_stream << endl;
        }
        // Last line don't have a new line
        run_cvs_file_output_stream << string_helper::join(rows[data_size - 1], ",");
        run_cvs_file_output_stream.close();
    }

    cout << "Read '" << input_csv_file_name << "' is done!" << endl;
    cout << "Entire process so far took a total of: " << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000
         << " milliseconds." << endl;
    cout << "-------------------------------------------------------\n\n" << endl;

    return run_count;
}

void merge_csv_files(int start, int end, int location, const vector<int> &sort_array) {

    const int runs_count = end - start + 1;

    ifstream input[runs_count];
    for (int i = 0; i < runs_count; i++) {
        stringstream string_stream;
        string_stream << "run_" << start + i << ".csv";
        input[i].open(string_stream.str());
    }

    // A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element,
    // at the expense of logarithmic insertion and extraction.
    priority_queue<HeapNode, vector<HeapNode> > heap;

    ofstream cvs_log_output_stream;
    stringstream string_stream;
    string_stream << "run_" << location << ".csv";
    cvs_log_output_stream.open(string_stream.str());

    for (int index = 0; index < runs_count; index++) {
        string line;
        if (!input[index].eof()) {
            getline(input[index], line);
            heap.emplace(line, index, sort_array);
        }
    }

    cout << "-------------------------------------------------------" << endl;
    cout << endl << "Merging from run_" << start << " to run_" << end << " into run_" << location << " file" << endl;

    while (!heap.empty()) {
        string line = heap.top().sentence;
        int index = heap.top().index;
        heap.pop();

        cvs_log_output_stream << line << endl;

        if (!input[index].eof()) {
            getline(input[index], line);
            heap.emplace(line, index, sort_array);
        }
    }

    cout << "Merge done!\n" << endl;
    cout << "-------------------------------------------------------\n\n" << endl;

    for (int i = 0; i < runs_count; i++)
        input[i].close();

    cvs_log_output_stream.close();
}

void merge_cvs_files(const int runs_count, const string &output_name, const vector<int> &sort_array) {

    cout << "-------------------------------------------------------" << endl;
    cout << "Merging " << runs_count << " files into output (" << output_name << " file)" << endl;
    cout << "-------------------------------------------------------\n\n" << endl;

    int start = 1;
    int end = runs_count;
    while (start < end) {
        int location = end;
        int distance = 100;
        int time = (end - start + 1) / distance + 1;
        if ((end - start + 1) / time < distance)
            distance = (end - start + 1) / time + 1;
        while (start <= end) {
            int mid = min(start + distance, end);
            location++;
            merge_csv_files(start, mid, location, sort_array);
            start = mid + 1;
        }
        end = location;
    }

    stringstream string_stream;
    string_stream << "run_" << start << ".csv";
    rename(string_stream.str().c_str(), output_name.c_str());

    cout << "-------------------------------------------------------" << endl;
    cout << "Removing chucks files!" << endl;
    for (int i = 1; i < end; i++) {
        string_stream.clear();
        string_stream << "run_" << i << ".csv";
        cout << "Removing " << string_stream.str() << endl;
        filesystem::remove(string_stream.str().c_str());
    }
    cout << "-------------------------------------------------------\n\n" << endl;
}

int main(const int argc, const char *argv[]) {
    if (argc > 0) {
        const string input_name = argv[1];

        if (argc == 3) {
            const long total_mem = strtol(argv[2], nullptr, 0); // bytes
            generate_csv_log_file(input_name, total_mem);

            return 0;
        } else if (argc == 4) {
            const string output_name = argv[2];
            const long total_mem = strtol(argv[3], nullptr, 0); // bytes
            const ifstream cvs_log_file_stream(input_name.c_str());
            if (!cvs_log_file_stream.good()) {
                generate_csv_log_file(input_name, total_mem);
            }
            // sort index for cvs column default to asc
            const vector<int> sort_array = {2, 1};

            const int runs_count = input_cvs_file(input_name, total_mem, sort_array);

            merge_cvs_files(runs_count, output_name, sort_array);

            cout << "Entire process took a total of: " << float(clock() - begin_time) / CLOCKS_PER_SEC * 1000
                 << " milliseconds." << endl;

            return 0;
        }
    }

    cout << "To generate input file: input_file mem_size" << endl <<
         "Or to sort extra large file: input_file output_file mem_size" << endl <<
         "Note: mem_size in bytes such as 1048576 (1MB)" << endl <<
         "Exit program!" << endl;
    return -1;
}
