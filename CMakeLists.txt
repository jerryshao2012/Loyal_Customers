cmake_minimum_required(VERSION 3.26)
project(Test)

set(CMAKE_CXX_STANDARD 17)

add_executable(Main main.cpp
        string_helper.cpp
        string_helper.h)

add_executable(GetLoyalCustomersUsingSet get_loyal_customers_using_set.cpp
        string_helper.cpp
        string_helper.h)

add_executable(GetLoyalCustomersUsingSortedFile get_loyal_customers_using_sorted_file.cpp
        string_helper.cpp
        string_helper.h
        cvs_helper.cpp
        cvs_helper.h)

add_executable(ExternalSortingCSV external_sorting_csv.cpp
        string_helper.cpp
        string_helper.h)
