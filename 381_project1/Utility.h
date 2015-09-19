#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>

#define INPUT_BUFFER_SIZE 1024
#define MAX_INPUT 63

// Pair of macros for making a macro a string
#define STRINGIFYHELPER(val) #val
#define STRINGIFY_MACRO(val) STRINGIFYHELPER(val)

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

#endif // UTILITY_H
