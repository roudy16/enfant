#include "Utility.h"
#include "Person.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int g_string_memory = 0;

void free_string(char* string_ptr){
    if (string_ptr){
        string_ptr = string_ptr - sizeof(int);
        g_string_memory -= *(int*)string_ptr;
        free(string_ptr);
    }
}

const char* create_string(const char* const string_ptr){
    int string_bytes = strlen(string_ptr) + 1;
    void* new_string_ptr = malloc(string_bytes + sizeof(int));

    // Check if malloc was successful
    if (!new_string_ptr){
        return new_string_ptr;
    }

    // Store num bytes used for string and increment string mem counter
    g_string_memory += ((*(int*)new_string_ptr = string_bytes));
    new_string_ptr = (char*)new_string_ptr + sizeof(int);
    strncpy(new_string_ptr, string_ptr, string_bytes);

    return new_string_ptr;
}

int discard_rest_of_input_line(FILE* infile){
    return fscanf(infile, "%*[^\n]%c");
}

int person_comp(const void* person_ptr1, const void* person_ptr2){
    return strcmp(get_Person_lastname((struct Person *const)person_ptr1),
                  get_Person_lastname((struct Person *const)person_ptr2));
}
