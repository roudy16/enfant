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

int g_number_Person_structs = 0;
int g_number_Room_structs = 0;

struct Schedule{
    struct Ordered_container* rooms_ptr;
    struct Ordered_container* people_ptr;
};

static void deallocate_all(struct Schedule* const schedule_ptr);

static int room_comp(const struct Room *const room_ptr1,
                     const struct Room *const room_ptr2)
{
    return get_Room_number(room_ptr1) - get_Room_number(room_ptr2);
}

static int room_comp_to_number(const int* number_ptr,
                               const struct Room* room_ptr)
{
    return *number_ptr - get_Room_number(room_ptr);
}

struct Schedule* const create_schedule(void){
    struct Schedule *const schedule_ptr = malloc(sizeof(struct Schedule));

    if (!schedule_ptr){
        perror("Failed to allocate schedule object");
        return NULL;
    }

    schedule_ptr->rooms_ptr = OC_create_container(&room_comp);
    if (!schedule_ptr->rooms_ptr)
    {
        free(schedule_ptr);
        perror("Failed to create rooms container");
        return NULL;
    }

    schedule_ptr->people_ptr = OC_create_container(&person_comp);
    if (!schedule_ptr->people_ptr)
    {
        OC_destroy_container(schedule_ptr->rooms_ptr);
        free(schedule_ptr);
        perror("Failed to create people container");
        return NULL;
    }

    return schedule_ptr;
}

static void destroy_schedule(struct Schedule* schedule_ptr)
{
    assert(schedule_ptr);

    deallocate_all(schedule_ptr);
    free(schedule_ptr);

    assert(g_Container_count == 0);
    assert(g_Meeting_memory == 0);
    assert(g_string_memory == 0);
    assert(g_Container_items_allocated == 0);
    assert(g_Container_items_in_use == 0);
}

static int get_command_from_input(char* command1, char* command2)
{
    printf("\nEnter command: ");
    int return_val = scanf(" %c %c", command1, command2);

    assert(return_val > 0);

    return return_val;
}

static void* find_object_arg(const struct Ordered_container* c_ptr,
                             const void* arg_ptr,
                             OC_find_item_arg_fp_t comp_func)
{
    void* item_ptr = OC_find_item_arg(c_ptr, arg_ptr, comp_func);
    void* data_ptr = NULL;

    if (item_ptr)
    {
        data_ptr = OC_get_data_ptr(item_ptr);
    }

    return data_ptr;
}

static struct Person* find_person_by_name(const struct Ordered_container* c_ptr,
                                          const char* name)
{
    return find_object_arg(c_ptr, name, (OC_find_item_arg_fp_t)person_to_name_comp);
}

static void read_string_from_input(char* string_buffer)
{
    int return_val = scanf("%" STRINGIFY_MACRO(MAX_INPUT) "s", string_buffer);

    if (return_val <= 0)
    {
        assert("Failed to read string from input");
    }
}

static int read_int_from_input(int* int_ptr)
{
    int return_val = scanf("%d", int_ptr);

    if (return_val <= 0){
        printf("Could not read an integer value!\n");
        discard_rest_of_input_line(stdin);
        return FAILURE;
    }
    return SUCCESS;
}

static void print_person_command(struct Schedule *const schedule_ptr)
{
    static char lastname_buffer[MAX_INPUT + 1];

    read_string_from_input(lastname_buffer);

    struct Person* person_ptr = find_person_by_name(schedule_ptr->people_ptr,
                                                    lastname_buffer);
    if (!person_ptr)
    {
        printf("No person with that name!\n");
        discard_rest_of_input_line(stdin);
    }
    else
    {
        print_Person(person_ptr);
    }
}

static int check_room_range_valid(const int room_number)
{
    return room_number > 0;
}

static int check_time_range_valid(int time)
{
    time = convert_time_to_24_hour(time);
    return time >= MEETING_TIME_LOW && time <= MEETING_TIME_HIGH;
}

static int read_room_from_input(int* room_number_ptr)
{
    if (read_int_from_input(room_number_ptr) != SUCCESS)
    {
        return FAILURE;
    }

    if (!check_room_range_valid(*room_number_ptr))
    {
        printf("Room number is not in range!\n");
        discard_rest_of_input_line(stdin);
        return FAILURE;
    }

    return SUCCESS;
}

static int read_time_from_input(int* time_ptr)
{
    if (read_int_from_input(time_ptr) != SUCCESS)
    {
        return FAILURE;
    }

    if (!check_time_range_valid(*time_ptr))
    {
        printf("Time is not in range!\n");
        discard_rest_of_input_line(stdin);
        return FAILURE;
    }

    return SUCCESS;
}

static int check_input_room_found(const struct Room *const room_ptr)
{
    if (!room_ptr)
    {
        printf("No room with that number!\n");
        discard_rest_of_input_line(stdin);
    }

    return (int)room_ptr;
}

static int check_input_meeting_found(const struct Meeting *const meeting_ptr)
{
    if (!meeting_ptr)
    {
        printf("No meeting at that time!\n");
        discard_rest_of_input_line(stdin);
    }

    return (int)meeting_ptr;
}

static void print_room_command(struct Schedule *const schedule_ptr)
{
    int room_number = -1;

    if (read_room_from_input(&room_number) != SUCCESS){
        return;
    }

    const struct Room* const room_ptr = find_object_arg(schedule_ptr->rooms_ptr,
                                                        &room_number,
                                                        (OC_find_item_arg_fp_t)room_comp_to_number);
    if (check_input_room_found(room_ptr))
    {
        print_Room(room_ptr);
    }
}

static void print_meeting_command(struct Schedule *const schedule_ptr)
{
    int room_number = -1;
    int meeting_time = -1;

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }

    const struct Room* const room_ptr = find_object_arg(schedule_ptr->rooms_ptr,
                                                        &room_number,
                                                        (OC_find_item_arg_fp_t)room_comp_to_number);
    if (!check_input_room_found(room_ptr))
    {
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    const struct Meeting* const meeting_ptr = find_Room_Meeting(room_ptr, meeting_time);

    if (check_input_meeting_found(meeting_ptr))
    {
        print_Meeting(meeting_ptr);
    }
}

static void print_all_meetings(struct Schedule *const schedule_ptr)
{
    int number_of_rooms = OC_get_size(schedule_ptr->rooms_ptr);

    if (number_of_rooms == 0)
    {
        printf("List of rooms is empty\n");
    }
    else
    {
        printf("Information for %d rooms:\n", number_of_rooms);
        OC_apply(schedule_ptr->rooms_ptr, (OC_apply_fp_t)print_Room);
    }
}

static void print_all_people(struct Schedule *const schedule_ptr)
{
    int number_of_people = OC_get_size(schedule_ptr->people_ptr);

    if (number_of_people == 0)
    {
        printf("List of people is empty\n");
    }
    else
    {
        printf("Information for %d people:\n", number_of_people);
        OC_apply(schedule_ptr->people_ptr, (OC_apply_fp_t)print_Person);
    }
}

static void print_memory_allocations(void)
{
    printf("Memory allocations:\n");
    printf("C-strings: %d bytes total\n", g_string_memory);
    printf("Person structs: %d\n", g_number_Person_structs);
    printf("Meeting structs : %d\n", g_Meeting_memory);
    printf("Room structs: %d\n", g_number_Room_structs);
    printf("Containers: %d\n", g_Container_count);
    printf("Container items in use: %d\n", g_Container_items_in_use);
    printf("Container items allocated: %d\n", g_Container_items_allocated);
}

static void add_to_people_list(struct Schedule *const schedule_ptr)
{
    struct Person* person_ptr = load_Person(stdin);
    assert(person_ptr);

    void* item_ptr = OC_find_item(schedule_ptr->people_ptr, person_ptr);
    if (item_ptr){
        printf("There is already a person with this last name!\n");
        discard_rest_of_input_line(stdin);
        destroy_Person(person_ptr);
    }
    else
    {
        OC_insert(schedule_ptr->people_ptr, person_ptr);
        printf("Person %s added\n", get_Person_lastname(person_ptr));
    }
}

static void print_switch(char command, struct Schedule *const schedule_ptr){
    switch (command)
    {
        case 'i':
            print_person_command(schedule_ptr);
            break;
        case 'r':
            print_room_command(schedule_ptr);
            break;
        case 'm':
            print_meeting_command(schedule_ptr);
            break;
        case 's':
            print_all_meetings(schedule_ptr);
            break;
        case 'g':
            print_all_people(schedule_ptr);
            break;
        case 'a':
            print_memory_allocations();
            break;
        default:
            printf("Unrecognized command!\n");
    }
}

static void add_switch(char command, struct Schedule *const schedule_ptr)
{
    switch (command)
    {
        case 'i':
            add_to_people_list(schedule_ptr);
            break;
        case 'r':
            break;
        case 'm':
            break;
        case 'p':
            break;
        default:
            printf("Unrecognized command!\n");
    }
}

static void deallocate_all(struct Schedule* const schedule_ptr)
{
    OC_apply(schedule_ptr->rooms_ptr, &destroy_Room);
    OC_destroy_container(schedule_ptr->rooms_ptr);
    OC_apply(schedule_ptr->people_ptr, &destroy_Person);
    OC_destroy_container(schedule_ptr->people_ptr);
}

void main_loop(struct Schedule *const schedule_ptr){
    int quit_flag = 0;
    char command1 = '\0';
    char command2 = '\0';

    while (!quit_flag){
        get_command_from_input(&command1, &command2);

        switch (command1){
            case 'p':
                print_switch(command2, schedule_ptr);
                break;
            case 'a':
                add_switch(command2, schedule_ptr);
                break;
            case 'r':
            case 'd':
            case 's':
            case 'l':
            case 'q':
                if (command2 == 'q')
                {
                    quit_flag = 1;
                    continue;
                }
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