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
    // Calculate bytes need to hold each string plus null char
    const int firstname_size = (int)strlen(firstname) + 1;
    const int lastname_size = (int)strlen(lastname) + 1;
    const int string_bytes = firstname_size + lastname_size + (int)strlen(phoneno) + 1;

    struct Person* person_ptr = malloc(sizeof(struct Person) + string_bytes);
    g_string_memory += string_bytes;

    person_ptr->firstname = ((char*)person_ptr) + sizeof(struct Person);
    person_ptr->lastname = person_ptr->lastname + firstname_size;
    person_ptr->phoneno = person_ptr->lastname + lastname_size;

    strcpy(person_ptr->firstname, firstname);
    strcpy(person_ptr->lastname, lastname);
    strcpy(person_ptr->phoneno, phoneno);

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
