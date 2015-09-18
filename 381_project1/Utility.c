#include "Utility.h"
#include <stdlib.h>
#include <string.h>

int g_string_memory = 0;

void free_string(char* string_ptr){
    g_string_memory -= (int)strlen(string_ptr);
    free(string_ptr);
}
