#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>

#define MAX_INPUT 63
#define MEETING_TIME_LOW 9   // 24-hour
#define MEETING_TIME_HIGH 17 // 24-hour

// Pair of macros for making a macro a string literal
#define STRINGIFYHELPER(val) #val
#define STRINGIFY_MACRO(val) STRINGIFYHELPER(val)

#define FAILURE -1
#define SUCCESS 0

// Forward declaration of types for function prototypes
struct Person;
struct Room;
struct Meeting;

/* global variable declarations for this module */
extern int g_string_memory;     /* number of bytes used in C-strings */

// Creates a new string that holds a copy of the string passed in
// Strings created using create_string() must be freed using free_string()
const char* create_string(const char* const string_ptr);

// Frees memory allocated for strings created using create_string()
// The behavior for this function is only defined for string created with
// create_string()
void free_string(char* string_ptr);


int discard_rest_of_input_line(FILE* infile);

int person_comp(const struct Person *const person_ptr1,
                const struct Person *const person_ptr2);

int person_to_name_comp(const char *const name_ptr,
                        const struct Person *const person_ptr);

int meeting_comp_to_time(const int* time_ptr, const struct Meeting* meeting_ptr);

int convert_time_to_24_hour(int time);

#endif // UTILITY_H
