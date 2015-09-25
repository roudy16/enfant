#include "Person.h"
#include "Utility.h"
#include "p1_globals.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* a Person consist of pointers to C-strings for names and phone number */
struct Person {
	char* firstname;
	char* lastname;
	char* phoneno;
};

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

        ++g_number_Person_structs;
    }

    return person_ptr;
}

void destroy_Person(struct Person* person_ptr){
    if (person_ptr){
        free_string(person_ptr->firstname);
        free_string(person_ptr->lastname);
        free_string(person_ptr->phoneno);
        free(person_ptr);
        --g_number_Person_structs;
    }
}

const char* get_Person_lastname(const struct Person* person_ptr){
    return person_ptr->lastname;
}

void print_Person(const struct Person* person_ptr){
    output_Person(person_ptr, stdout);
}

void save_Person(const struct Person* person_ptr, FILE* outfile){
    output_Person(person_ptr, outfile);
    fflush(outfile); // Ensures data is actually written
}

struct Person* load_Person(FILE* infile){
    static char firstname[MAX_INPUT + 1];
    static char lastname[MAX_INPUT + 1];
    static char phoneno[MAX_INPUT + 1];

    const int return_val = fscanf(infile, "%" STRINGIFY_MACRO(MAX_INPUT) "s"
                                  "%" STRINGIFY_MACRO(MAX_INPUT) "s"
                                  "%" STRINGIFY_MACRO(MAX_INPUT) "s",
                                  firstname, lastname, phoneno);

    struct Person* person = NULL;
    if (return_val != 3){
        discard_rest_of_input_line(infile);
    }
    else {
        person = create_Person(firstname, lastname, phoneno);
    }
    return person;
}
