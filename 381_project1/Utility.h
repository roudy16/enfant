#ifndef UTILITY_H
#define UTILITY_H

#define MAX_INPUT 64

/* global variable declarations for this module */
extern int g_string_memory;     /* number of bytes used in C-strings */

void free_string(char* string_ptr);

#endif // UTILITY_H
