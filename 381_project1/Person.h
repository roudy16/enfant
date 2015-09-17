#ifndef PERSON_H
#define PERSON_H

/* 
A Person is an opaque type containing a first name, last name, and phone number 
as pointers to C-strings stored in dynamically allocated memory.
*/

#include <stdio.h> /* for the declaration of FILE*/

/* incomplete declaration */
struct Person;

/* Create a Person object.
This is the only function that allocates dynamic memory for a Person
and the contained data. */
struct Person* create_Person(const char* firstname, const char* lastname, const char* phoneno);

/* Destroy a Person object
This is the only function that frees the memory for a Person
and the contained data. */
void destroy_Person(struct Person* person_ptr);

/* Return the lastname pointer as a pointer to unmodifiable characters. */
const char* get_Person_lastname(const struct Person* person_ptr);

/* Print a Person data item to standard output with a final \n character. */
void print_Person(const struct Person* person_ptr);

/* Write a Person to a file stream, as a line containing 
firstname, lastname, phoneno,  whitespace-separated with a final \n character. */
void save_Person(const struct Person* person_ptr, FILE* outfile);

/* Read a Person's data from a file stream, create the data object and 
return a pointer to it, NULL if invalid data discovered in file.
No check made for whether the Person already exists or not. */
struct Person* load_Person(FILE* infile);


#endif
