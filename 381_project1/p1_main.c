#include "Utility.h"
#include "Ordered_container.h"
#include "Room.h"
#include "Person.h"
#include "Meeting.h"
#include "p1_globals.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct Schedule{
    struct Ordered_container* rooms_ptr;
    struct Ordered_container* people_ptr;
};

static int room_comp(const struct Room *const room_ptr1,
                     const struct Room *const room_ptr2)
{
    return get_Room_number(room_ptr1) - get_Room_number(room_ptr2);
}

struct Schedule* const create_schedule(void){
    struct Schedule *const schedule_ptr = malloc(sizeof(struct Schedule));

    if (!schedule_ptr){
        perror("Failed to allocate schedule object");
        return NULL;
    }

    schedule_ptr->rooms_ptr = OC_create_container(&room_comp);
    if (!schedule_ptr->rooms_ptr){
        free(schedule_ptr);
        perror("Failed to create rooms container");
        return NULL;
    }

    schedule_ptr->people_ptr = OC_create_container(&person_comp);
    if (!schedule_ptr->people_ptr){
        OC_destroy_container(schedule_ptr->rooms_ptr);
        free(schedule_ptr);
        perror("Failed to create people container");
        return NULL;
    }

    return schedule_ptr;
}

static void destroy_schedule(struct Schedule* schedule_ptr){
    assert(schedule_ptr);

    OC_apply(schedule_ptr->rooms_ptr, &destroy_Room);
    OC_destroy_container(schedule_ptr->rooms_ptr);
    OC_apply(schedule_ptr->people_ptr, &destroy_Person);
    OC_destroy_container(schedule_ptr->people_ptr);

    free(schedule_ptr);

    assert(g_Container_count == 0);
    assert(g_Meeting_memory == 0);
    assert(g_string_memory == 0);
    assert(g_Container_items_allocated == 0);
    assert(g_Container_items_in_use == 0);
}

static int get_command_from_input(char* command1, char* command2){
    printf("\nEnter command: ");
    int return_val = scanf("% c% c", command1, command2);

    assert(return_val > 0);

    return return_val;
}

static void print_person_command(struct Schedule *const schedule_ptr){
    static char lastname_buffer[MAX_INPUT + 1];

    int return_val = scanf("%" STRINGIFY_MACRO(MAX_INPUT) "s", lastname_buffer);

    if (return_val <= 0){
        assert("Failed to read lastname in print_person_command");
    }

    void* item_ptr = OC_find_item_arg(schedule_ptr->people_ptr, lastname_buffer,
                                      &person_to_name_comp);

    if (!item_ptr){
        printf("No person with that name!\n");
        discard_rest_of_input_line(stdin);
    }
    else {
    }


}

static void print_room_command(struct Schedule *const schedule_ptr){

}

static void print_switch(char command, struct Schedule *const schedule_ptr){
    switch (command){
        case 'i':
            print_person_command(schedule_ptr);
            break;
        case 'r':
            print_room_command(schedule_ptr);
            break;
        case 'm':
            break;
        case 's':
            break;
        case 'g':
            break;
        case 'a':
            break;
        default:
            printf("Unrecognized command!\n");
    }
}

void main_loop(struct Schedule *const schedule_ptr){
    char command1 = '\0';
    char command2 = '\0';

    while (1){
        get_command_from_input(&command1, &command2);

        switch (command1){
            case 'p':
                print_switch(command2, schedule_ptr);
                break;
            case 'a':
            case 'r':
            case 'd':
            case 's':
            case 'l':
            default:
                printf("Unrecognized command!\n");
        }
    }
}


int main(void){
    struct Schedule *const schedule_ptr = create_schedule();
    assert(schedule_ptr);

    main_loop(schedule_ptr);

    destroy_schedule(schedule_ptr);
    return 0;
}