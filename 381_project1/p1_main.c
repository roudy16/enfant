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


static void print_switch(char command, struct Schedule *const schedule_ptr);

static void add_switch(char command, struct Schedule *const schedule_ptr);

static void delete_switch(char command, struct Schedule *const schedule_ptr);

static int room_comp(const struct Room *const room_ptr1,
                     const struct Room *const room_ptr2);

static int room_comp_to_number(const int* number_ptr,
                               const struct Room* room_ptr);

static struct Schedule* const create_schedule(void);

static void destroy_schedule(struct Schedule* schedule_ptr, const int quiet);

static int get_command_from_input(char* command1, char* command2);

static void* find_object_arg(const struct Ordered_container* c_ptr,
                             const void* arg_ptr,
                             OC_find_item_arg_fp_t comp_func);

static struct Person* find_person_by_name(const struct Ordered_container* c_ptr,
                                          const char* name);

static struct Room* find_room_by_number(const struct Ordered_container* c_ptr,
                                        const int* room_number_ptr);

static void read_string_from_input(char* string_buffer);

static int read_int_from_input(int* int_ptr);

static int check_room_range_valid(const int room_number);

static void print_person_command(struct Schedule *const schedule_ptr);

static int check_time_range_valid(int time);

static void room_not_found_error();

static void person_not_found_error();

static void meeting_not_found_error();

static void unrecognized_command_error();

static int read_room_from_input(int* room_number_ptr);

static int read_time_from_input(int* time_ptr);

static void print_room_command(struct Schedule *const schedule_ptr);

static void print_meeting_command(struct Schedule *const schedule_ptr);

static void print_all_meetings(struct Schedule *const schedule_ptr);

static void print_all_people(struct Schedule *const schedule_ptr);

static void print_memory_allocations(void);

static void add_to_people_list(struct Schedule *const schedule_ptr);

static void add_room(struct Schedule *const schedule_ptr);

static void add_meeting(struct Schedule *const schedule_ptr);

static void add_person_to_meeting_in_room(struct Schedule *const schedule_ptr);

static void reschedule_meeting(struct Schedule *const schedule_ptr);

static void delete_individual_command(struct Schedule *const schedule_ptr);

static void delete_room_command(struct Schedule *const schedule_ptr);

static void delete_meeting_command(struct Schedule *const schedule_ptr);

static void delete_participant_command(struct Schedule *const schedule_ptr);

static void delete_all_individual_command(struct Schedule *const schedule_ptr);

static void delete_schedule_command(struct Schedule *const schedule_ptr,
                                    const int quiet);

static void deallocate_all(struct Schedule *const schedule_ptr, const int quiet);

static void save_data_command(struct Schedule *const schedule_ptr);

static void load_data_command(struct Schedule** schedule_ptr);

int main(void){
    struct Schedule* schedule_ptr = create_schedule();
    assert(schedule_ptr);

    int quit_flag = 0;
    char command1 = '\0';
    char command2 = '\0';

    while (!quit_flag)
    {
        get_command_from_input(&command1, &command2);

        switch (command1)
        {
            case 'p':
                print_switch(command2, schedule_ptr);
                break;
            case 'a':
                add_switch(command2, schedule_ptr);
                break;
            case 'r':
                if (command2 == 'm')
                {
                    reschedule_meeting(schedule_ptr);
                }
                else
                {
                    unrecognized_command_error();
                }
                break;
            case 'd':
                delete_switch(command2, schedule_ptr);
                break;
            case 's':
                if (command2 == 'd')
                {
                    save_data_command(schedule_ptr);
                }
                else
                {
                    unrecognized_command_error();
                }
                break;
            case 'l':
                if (command2 == 'd')
                {
                    load_data_command(&schedule_ptr);
                }
                else
                {
                    unrecognized_command_error();
                }
                break;
            case 'q':
                if (command2 == 'q')
                {
                    quit_flag = 1;
                    continue;
                }
                /* no break */
            default:
                unrecognized_command_error();
        }
    }

    destroy_schedule(schedule_ptr, 0);
    assert(g_Container_count == 0);
    assert(g_Meeting_memory == 0);
    assert(g_string_memory == 0);
    assert(g_Container_items_allocated == 0);
    assert(g_Container_items_in_use == 0);

    printf("Done\n");
    return 0;
}

static void print_switch(char command, struct Schedule *const schedule_ptr)
{
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
            unrecognized_command_error();
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
            add_room(schedule_ptr);
            break;
        case 'm':
            add_meeting(schedule_ptr);
            break;
        case 'p':
            add_person_to_meeting_in_room(schedule_ptr);
            break;
        default:
            unrecognized_command_error();
    }
}

static void delete_switch(char command, struct Schedule *const schedule_ptr)
{
    switch (command)
    {
        case 'i':
            delete_individual_command(schedule_ptr);
            break;
        case 'r':
            delete_room_command(schedule_ptr);
            break;
        case 'm':
            delete_meeting_command(schedule_ptr);
            break;
        case 'p':
            delete_participant_command(schedule_ptr);
            break;
        case 's':
            delete_schedule_command(schedule_ptr, 0);
            break;
        case 'g':
            delete_all_individual_command(schedule_ptr);
            break;
        case 'a':
            deallocate_all(schedule_ptr, 0);
            break;
        default:
            unrecognized_command_error();
    }
}

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

struct Schedule* const create_schedule(void)
{
    struct Schedule *const schedule_ptr = malloc(sizeof(struct Schedule));

    if (!schedule_ptr){
        perror("Failed to allocate schedule object");
        return NULL;
    }

    schedule_ptr->rooms_ptr = OC_create_container((OC_comp_fp_t)room_comp);
    if (!schedule_ptr->rooms_ptr)
    {
        free(schedule_ptr);
        perror("Failed to create rooms container");
        return NULL;
    }

    schedule_ptr->people_ptr = OC_create_container((OC_comp_fp_t)person_comp);
    if (!schedule_ptr->people_ptr)
    {
        OC_destroy_container(schedule_ptr->rooms_ptr);
        free(schedule_ptr);
        perror("Failed to create people container");
        return NULL;
    }

    return schedule_ptr;
}

static void destroy_schedule(struct Schedule* schedule_ptr, const int quiet)
{
    assert(schedule_ptr);

    deallocate_all(schedule_ptr, quiet);
    OC_destroy_container(schedule_ptr->rooms_ptr);
    OC_destroy_container(schedule_ptr->people_ptr);
    free(schedule_ptr);

}

static int get_command_from_input(char* command1, char* command2)
{
    printf("\nEnter command: ");
    int return_val = scanf(" %c %c", command1, command2);

    assert(return_val == 2);

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

static struct Room* find_room_by_number(const struct Ordered_container* c_ptr,
                                        const int* room_number_ptr)
{
    return find_object_arg(c_ptr, room_number_ptr,
                           (OC_find_item_arg_fp_t)room_comp_to_number);
}

static void read_string_from_input(char* string_buffer)
{
    int return_val = scanf("%" STRINGIFY_MACRO(MAX_INPUT) "s", string_buffer);
 if (return_val != 1)
    {
        assert("Failed to read string from input");
    }
}

static int read_int_from_input(int* int_ptr)
{
    int return_val = scanf("%d", int_ptr);

    if (return_val != 1){
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

static void room_not_found_error()
{
    printf("No room with that number!\n");
    discard_rest_of_input_line(stdin);
}

static void person_not_found_error()
{
    printf("No person with that name!\n");
    discard_rest_of_input_line(stdin);
}

static void meeting_not_found_error()
{
    printf("No meeting at that time!\n");
    discard_rest_of_input_line(stdin);
}

static void unrecognized_command_error()
{
    printf("Unrecognized command!\n");
    discard_rest_of_input_line(stdin);
}

static void print_room_command(struct Schedule *const schedule_ptr)
{
    int room_number = -1;

    if (read_room_from_input(&room_number) != SUCCESS){
        return;
    }

    const struct Room* const room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                            &room_number);
    if (room_ptr)
    {
        print_Room(room_ptr);
    }
    else
    {
        room_not_found_error();
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

    const struct Room* const room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                            &room_number);
    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    const struct Meeting* const meeting_ptr = find_Room_Meeting(room_ptr, meeting_time);

    if (meeting_ptr)
    {
        print_Meeting(meeting_ptr);
    }
    else
    {
        meeting_not_found_error();
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
    printf("Meeting structs: %d\n", g_Meeting_memory);
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

static void add_room(struct Schedule *const schedule_ptr)
{
    int room_number = 0;
    if (read_room_from_input(&room_number) == SUCCESS)
    {
        struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr, &room_number);
        if (room_ptr)
        {
            printf("There is already a room with this number!\n");
            discard_rest_of_input_line(stdin);
            return;
        }
        else
        {
            room_ptr = create_Room(room_number);
            OC_insert(schedule_ptr->rooms_ptr, room_ptr);
            printf("Room %d added\n", room_number);
        }
    }
}

static void add_meeting(struct Schedule *const schedule_ptr)
{
    int room_number = 0;
    int meeting_time = 0;
    char topic[MAX_INPUT + 1];

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }

    struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr, &room_number);

    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    read_string_from_input(topic);
    struct Meeting* meeting_ptr = create_Meeting(meeting_time, topic);
    if (add_Room_Meeting(room_ptr, meeting_ptr))
    {
        destroy_Meeting(meeting_ptr);
        printf("There is already a meeting at that time!\n");
        discard_rest_of_input_line(stdin);
        return;
    }
    else
    {
        printf("Meeting added at %d\n", meeting_time);
    }
}

static void add_person_to_meeting_in_room(struct Schedule *const schedule_ptr)
{
    int room_number = 0;
    int meeting_time = 0;
    char lastname[MAX_INPUT + 1];

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }


    struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                &room_number);
    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    struct Meeting* meeting_ptr = find_Room_Meeting(room_ptr, meeting_time);
    if (!meeting_ptr)
    {
        meeting_not_found_error();
        return;
    }

    read_string_from_input(lastname);

    struct Person* person_ptr = find_person_by_name(schedule_ptr->people_ptr,
                                                    lastname);
    if (!person_ptr)
    {
        person_not_found_error();
        return;
    }

    if (is_Meeting_participant_present(meeting_ptr, person_ptr))
    {
        printf("This person is already a participant!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    add_Meeting_participant(meeting_ptr, person_ptr);
    printf("Participant %s added\n", lastname);
}

static void reschedule_meeting(struct Schedule *const schedule_ptr)
{
    int old_room_number = 0;
    int old_meeting_time = 0;
    int new_room_number = 0;
    int new_meeting_time = 0;

    if (read_room_from_input(&old_room_number) != SUCCESS)
    {
        return;
    }

    struct Room* old_room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                &old_room_number);
    if (!old_room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&old_meeting_time) != SUCCESS)
    {
        return;
    }

    struct Meeting* meeting_ptr = find_Room_Meeting(old_room_ptr,
                                                    old_meeting_time);
    if (!meeting_ptr)
    {
        meeting_not_found_error();
        return;
    }

    if (read_room_from_input(&new_room_number) != SUCCESS)
    {
        return;
    }

    struct Room* new_room_ptr = NULL;
    if (new_room_number == old_room_number)
    {
        new_room_ptr = old_room_ptr;
    }
    else
    {
        new_room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                           &new_room_number);
        if (!new_room_ptr)
        {
            room_not_found_error();
            return;
        }
    }

    if (read_time_from_input(&new_meeting_time) != SUCCESS)
    {
        return;
    }

    struct Meeting* check_meeting_ptr = find_Room_Meeting(new_room_ptr,
                                                          new_meeting_time);
    if (check_meeting_ptr){
        printf("There is already a meeting at that time!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    int return_val = remove_Room_Meeting(old_room_ptr, meeting_ptr);
    assert(return_val == 0);
    set_Meeting_time(meeting_ptr, new_meeting_time);
    return_val = add_Room_Meeting(new_room_ptr, meeting_ptr);
    assert(return_val == 0);

    printf("Meeting rescheduled to room %d at %d\n", new_room_number,
                                                     new_meeting_time);
}

static int search_Room_for_person(const struct Room *const room_ptr,
                                  const struct Person *const person_ptr)
{
    int return_val = OC_apply_if_arg(get_Room_Meetings(room_ptr),
                                     (OC_apply_if_arg_fp_t)is_Meeting_participant_present,
                                     (void*)person_ptr);
    return return_val;
}

static void delete_individual_command(struct Schedule *const schedule_ptr)
{
    char lastname[MAX_INPUT + 1];

    read_string_from_input(lastname);

    struct Person* person_ptr = find_person_by_name(schedule_ptr->people_ptr,
                                                    lastname);
    if (!person_ptr){
        person_not_found_error();
        return;
    }

    int person_in_meeting = OC_apply_if_arg(schedule_ptr->rooms_ptr,
                                            (OC_apply_if_arg_fp_t)search_Room_for_person,
                                            person_ptr);
    if (person_in_meeting)
    {
        printf("This person is a participant in a meeting!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    void* item_ptr = OC_find_item_arg(schedule_ptr->people_ptr,
                                      person_ptr,
                                      (OC_find_item_arg_fp_t)person_comp);
    assert(item_ptr);

    OC_delete_item(schedule_ptr->people_ptr, item_ptr);
    destroy_Person(person_ptr);
    printf("Person %s deleted\n", lastname);
}

static void delete_room_command(struct Schedule *const schedule_ptr)
{
    int room_number = 0;

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }

    struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                &room_number);
    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    void* item_ptr = OC_find_item_arg(schedule_ptr->rooms_ptr,
                                      room_ptr,
                                      (OC_find_item_arg_fp_t)room_comp);
    assert(item_ptr);

    OC_delete_item(schedule_ptr->rooms_ptr, item_ptr);
    destroy_Room(room_ptr);
    printf("Room %d deleted\n", room_number);
}

static void delete_meeting_command(struct Schedule *const schedule_ptr)
{
    int room_number = 0;
    int meeting_time = 0;

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }

    struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                &room_number);
    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    struct Ordered_container* meetings_ptr = (struct Ordered_container*)get_Room_Meetings(room_ptr);

    void* meeting_item_ptr = OC_find_item_arg(meetings_ptr,
                                      &meeting_time,
                                      (OC_find_item_arg_fp_t)meeting_comp_to_time);

    if (!meeting_item_ptr)
    {
        meeting_not_found_error();
        return;
    }

    struct Meeting* meeting_ptr = OC_get_data_ptr(meeting_item_ptr);
    OC_delete_item(meetings_ptr, meeting_item_ptr);
    destroy_Meeting(meeting_ptr);
    printf("Meeting at %d deleted\n", meeting_time);
}

static void delete_participant_command(struct Schedule *const schedule_ptr)
{
    int room_number = 0;
    int meeting_time = 0;
    char lastname[MAX_INPUT + 1];

    if (read_room_from_input(&room_number) != SUCCESS)
    {
        return;
    }

    struct Room* room_ptr = find_room_by_number(schedule_ptr->rooms_ptr,
                                                &room_number);
    if (!room_ptr)
    {
        room_not_found_error();
        return;
    }

    if (read_time_from_input(&meeting_time) != SUCCESS)
    {
        return;
    }

    struct Meeting* meeting_ptr = find_Room_Meeting(room_ptr, meeting_time);
    if (!meeting_ptr)
    {
        meeting_not_found_error();
        return;
    }

    read_string_from_input(lastname);
    struct Person* person_ptr = find_person_by_name(schedule_ptr->people_ptr,
                                                    lastname);

    if (!person_ptr)
    {
        person_not_found_error();
        return;
    }

    int return_val = remove_Meeting_participant(meeting_ptr, person_ptr);
    if (return_val != 0)
    {
        printf("This person is not a participant in the meeting!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    printf("Participant %s deleted\n", lastname);
}

static int room_not_empty(const struct Room* const room_ptr)
{
    return !OC_empty(get_Room_Meetings(room_ptr));
}

static void delete_all_individual_command(struct Schedule *const schedule_ptr)
{
    int meetings_in_schedule = OC_apply_if(schedule_ptr->rooms_ptr,
                                           (OC_apply_if_fp_t)room_not_empty);
    if (meetings_in_schedule)
    {
        printf("Cannot clear people list unless there are no meetings!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    OC_apply(schedule_ptr->people_ptr, (OC_apply_fp_t)destroy_Person);
    OC_clear(schedule_ptr->people_ptr);
    printf("All persons deleted\n");
}

static void delete_schedule_command(struct Schedule *const schedule_ptr,
                                    const int quiet)
{
    OC_apply(schedule_ptr->rooms_ptr, (OC_apply_fp_t)clear_Room);
    if (!quiet)
    {
        printf("All meetings deleted\n");
    }
}

static void deallocate_all(struct Schedule* const schedule_ptr, const int quiet)
{
    delete_schedule_command(schedule_ptr, quiet);

    OC_apply(schedule_ptr->rooms_ptr, (OC_apply_fp_t)destroy_Room);
    OC_clear(schedule_ptr->rooms_ptr);
    if (!quiet)
    {
        printf("All rooms deleted\n");
    }

    OC_apply(schedule_ptr->people_ptr, (OC_apply_fp_t)destroy_Person);
    OC_clear(schedule_ptr->people_ptr);
    if (!quiet)
    {
        printf("All persons deleted\n");
    }
}

static void save_data_command(struct Schedule *const schedule_ptr)
{
    char filename[MAX_INPUT + 1];

    read_string_from_input(filename);

    FILE* savefile = fopen(filename, "w");
    if (!savefile)
    {
        printf("Could not open file!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    fprintf(savefile, "%d\n", g_number_Person_structs);
    OC_apply_arg(schedule_ptr->people_ptr, (OC_apply_arg_fp_t)save_Person, savefile);

    fprintf(savefile, "%d\n", g_number_Room_structs);
    OC_apply_arg(schedule_ptr->rooms_ptr, (OC_apply_arg_fp_t)save_Room, savefile);

    printf("Data saved\n");
    fclose(savefile);
}

static void load_data_error(struct Schedule* bad_schedule_ptr, FILE* loadfile)
{
    printf("Invalid data found in file!\n");

    if (bad_schedule_ptr)
    {
        destroy_schedule(bad_schedule_ptr, 1);
    }
    discard_rest_of_input_line(stdin);
    fclose(loadfile);
}

static void load_data_command(struct Schedule ** schedule_ptr)
{
    char filename[MAX_INPUT + 1];

    read_string_from_input(filename);

    FILE* loadfile = fopen(filename, "r");
    if (!loadfile)
    {
        printf("Could not open file!\n");
        discard_rest_of_input_line(stdin);
        return;
    }

    struct Schedule* new_schedule_ptr = NULL;
    int number_of_people = 0;

    int return_val = fscanf(loadfile, "%d", &number_of_people);
    if (return_val != 1)
    {
        load_data_error(new_schedule_ptr, loadfile);
        return;
    }

    new_schedule_ptr = create_schedule();

    // Load the people container
    for (int i = 0; i < number_of_people; ++i)
    {
        struct Person* new_person_ptr = load_Person(loadfile);
        if (!new_person_ptr)
        {
            load_data_error(new_schedule_ptr, loadfile);
            return;
        }

        OC_insert(new_schedule_ptr->people_ptr, new_person_ptr);
    }

    int number_of_rooms = 0;
    return_val = fscanf(loadfile, "%d", &number_of_rooms);
    if (return_val != 1)
    {
        load_data_error(new_schedule_ptr, loadfile);
        return;
    }

    // Load the rooms container
    for (int i = 0; i < number_of_rooms; ++i)
    {
        struct Room* room_ptr = load_Room(loadfile, new_schedule_ptr->people_ptr);
        if (!room_ptr)
        {
            load_data_error(new_schedule_ptr, loadfile);
            return;
        }

        OC_insert(new_schedule_ptr->rooms_ptr, room_ptr);
    }

    destroy_schedule(*schedule_ptr, 1);
    *schedule_ptr = new_schedule_ptr;
    printf("Data loaded\n");
    fclose(loadfile);
}


