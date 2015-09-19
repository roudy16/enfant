#include "Person.h"
#include "Utility.h"
#include <stdio.h>

int main(void){
    FILE* file_1 = fopen("test_person.out", "w");
    struct Person* me = create_Person("SteveSteveSteveSteveSteveSteveSteveSteveSteveSteveSteveSteve12345678", "Roudy", "312");
    print_Person(me);
    printf("My lastname is %s\n", get_Person_lastname(me));
    printf("String bytes allocated: %d\n", g_string_memory);
    save_Person(me, file_1);

    FILE* file_2 = fopen("test_person.out", "r");
    struct Person* me_too = load_Person(file_2);
    printf("String bytes allocated: %d\n", g_string_memory);
    destroy_Person(me);
    if (me_too){
        print_Person(me_too);
    }
    destroy_Person(me_too);
    printf("String bytes after destroy: %d\n", g_string_memory);
    fclose(file_1);
    fclose(file_2);
    return 0;
}