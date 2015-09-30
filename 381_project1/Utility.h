#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>

#define MAX_INPUT 63
#define EARLIEST_MEETING_TIME 9   // 24-hour
#define LATEST_MEETING_TIME 17 // 24-hour

// Pair of macros for making a macro a string literal
#define STRINGIFYHELPER(val) #val
#define STRINGIFY_MACRO(val) STRINGIFYHELPER(val)

// These values are used to check if a function's operations were
// successful. Functions that use these values are add/remove participant
// from meeting, add/remove meeting from room, input reading functions.
#define FAILURE -1
#define SUCCESS 0

// Forward declaration for function prototypes
struct Person;

/* global variable declarations for this module */
extern int g_string_memory;     /* number of bytes used in C-strings */

// Creates a new string that holds a copy of the string passed in
// Strings created using create_string() must be freed using free_string()
const char* create_string(const char* const string_ptr);

// Frees memory allocated for strings created using create_string()
// The behavior for this function is only defined for string created with
// create_string()
void free_string(char* string_ptr);

// When an error reading input is detected this function should be called
// to clear the rest of the offending input line
int discard_rest_of_input_line(FILE* infile);

// Comparison function to determine proper ordering of Person structs
int person_comp(const struct Person *const person_ptr1,
                const struct Person *const person_ptr2);

// Comparison function for comparing a Person struct to a name string
int person_to_name_comp(const char *const name_ptr,
                        const struct Person *const person_ptr);

// Returns a passed in 12-hour time from 9AM - 5PM to correct 24-hour
// time 9 - 17
int convert_time_to_24_hour(int time);


#endif // UTILITY_H
