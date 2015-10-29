#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <fstream>

class Person;

/* Utility functions, constants, and classes used by more than one other modules */

// Times in 24-hour format
const int k_EARLIEST_MEETING_TIME = 9; // 9am
const int k_LATEST_MEETING_TIME = 17;  // 5pm

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
    Error(const char* msg_ = "") :
        msg(msg_)
        {}
    const char* msg;
};

// TODO maybe a function to read single string from std::cin

template <typename T>
struct Less_than_ptr{
    bool operator()(const T lhs, const T rhs) const { return *lhs < *rhs; }
};

int convert_time_to_24_hour(int time);

std::string&& read_string_from_input();

void check_file_stream_status(std::ifstream& ifs);

//bool person_to_string_comp(std::string lastname, const Person *const person_ptr);


#endif // UTILITY_H
