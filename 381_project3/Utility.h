#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <fstream>

/* Utility functions, constants, and classes used by more than one other modules */

// Times in 24-hour format
const int k_EARLIEST_MEETING_TIME = 9; // 9am
const int k_LATEST_MEETING_TIME = 17;  // 5pm

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
    Error(const char* msg_);
    const char* msg;
};

// Comparator template for comparing objects with only a pointer
template <typename T>
struct Less_than_ptr {
    bool operator()(const T lhs, const T rhs) const { return *lhs < *rhs; }
};

// Convert a time integer from 12-hour to 24-hour time
int convert_time_to_24_hour(int time);

std::string read_string_from_stream(std::istream& is);

// Check stream status, throw Invalid data Error if status is not good
void check_file_stream_status(std::ifstream& ifs);

#endif // UTILITY_H
