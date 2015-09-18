#include "Person.h"
#include "Utility.h"
#include <stdio.h>

int main(void){
    struct Person* me = create_Person("Steve", "Roudy", "312");
    print_Person(me);
    printf("My lastname is %s\n", get_Person_lastname(me));
    printf("String bytes allocated: %d\n", g_string_memory);
    destroy_Person(me);
    printf("String bytes after destroy: %d\n", g_string_memory);

    return 0;
}