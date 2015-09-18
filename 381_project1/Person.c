#include "Person.h"
#include "Utility.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* a Person consist of pointers to C-strings for names and phone number */
struct Person {
	char* firstname;
	char* lastname;
	char* phoneno;
};

// TODO Add simple string creation utility and remove this, KISS i suppose
static const int Get_person_string_bytes(const struct Person *const person_ptr){
    return *(int*)(person_ptr + 1);
}

static void output_Person(const struct Person* const person_ptr, FILE* outfile){
    fprintf(outfile, "%s %s %s\n", person_ptr->firstname, person_ptr->lastname, person_ptr->phoneno);
}

struct Person* create_Person(const char* firstname,
                             const char* lastname,
                             const char* phoneno)
{
    struct Person* person_ptr = malloc(sizeof(struct Person));

    if (person_ptr){
        person_ptr->firstname = (char*)create_string(firstname);
        person_ptr->lastname = (char*)create_string(lastname);
        person_ptr->phoneno = (char*)create_string(phoneno);

        if (!person_ptr->firstname ||
            !person_ptr->lastname ||
            !person_ptr->phoneno)
        {
            destroy_Person(person_ptr);
            person_ptr = NULL;
        }
    }

    return person_ptr;
}

void destroy_Person(struct Person* person_ptr){
    free_string(person_ptr->firstname);
    free_string(person_ptr->lastname);
    free_string(person_ptr->phoneno);
    free(person_ptr);
}

const char* get_Person_lastname(const struct Person* person_ptr){
    return person_ptr->lastname;
}

void print_Person(const struct Person* person_ptr){
    output_Person(person_ptr, stdout);
}

void save_Person(const struct Person* person_ptr, FILE* outfile){
    output_Person(person_ptr, outfile);
}

struct Person* load_Person(FILE* infile);
