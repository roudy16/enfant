#ifndef UTILITY_H
#define UTILITY_H

#include <string>

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

struct LoadError {};

// TODO maybe a function to read single string from std::cin

template <typename T>
struct Less_than_ptr{
    bool operator()(const T lhs, const T rhs) const { return *lhs < *rhs; }
};

int convert_time_to_24_hour(int time);

//template <typename IT>
//void deallocate_in_range(IT& first, const IT& last) {
    //for_each(first, last, [](IT::value_type p){ delete p; });
//}

bool person_to_string_comp(std::string lastname, const Person *const person_ptr);


#endif // UTILITY_H
