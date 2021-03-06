#include "Ordered_container.h"
#include "Room.h"
#include "Meeting.h"
#include "Utility.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/* a Room contains a container of meetings and a room number */
struct Room {
	struct Ordered_container* meetings;	/* a container of pointers to Meeting objects */
	int number;
};


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// Takes in a true/false expression that is true if an error has occured in the
// file reading. Handles errors, returns result of the true/false expression
static int handle_load_room_error(const int bool_expr, FILE* input_file,
                                  struct Room** room_ptr);

// Compare function used to compare two Meeting structs by their times.
static int meeting_comp(const struct Meeting *const meeting1_ptr,
                        const struct Meeting *const meeting2_ptr);

// Compare function used to compare a Meeting struct to a time int
static int meeting_comp_to_time(const int* time_ptr,
                                const struct Meeting* meeting_ptr);


/* #################### */
/* FUNCTION DEFINITIONS */
/* #################### */


static int meeting_comp(const struct Meeting *const meeting1_ptr,
                        const struct Meeting *const meeting2_ptr)
{
    int time1 = get_Meeting_time(meeting1_ptr);
    int time2 = get_Meeting_time(meeting2_ptr);

    time1 = convert_time_to_24_hour(time1);
    time2 = convert_time_to_24_hour(time2);

    return time1 - time2;
}

static int meeting_comp_to_time(const int* time_ptr, const struct Meeting* meeting_ptr)
{
    int arg_time = convert_time_to_24_hour(*time_ptr);
    int meeting_time = convert_time_to_24_hour(get_Meeting_time(meeting_ptr));
    return arg_time - meeting_time;
}

struct Room* create_Room(int number)
{
    struct Room* new_room_ptr = malloc(sizeof(struct Room));

    if (!new_room_ptr)
    {
        perror("Could not allocate memory in create_Room\n");
        exit(EXIT_FAILURE);
    }

    new_room_ptr->meetings = OC_create_container((OC_comp_fp_t)meeting_comp);
    new_room_ptr->number = number;

    return new_room_ptr;
}

void destroy_Room(struct Room* room_ptr)
{
    if(room_ptr)
    {
        OC_apply(room_ptr->meetings, (OC_apply_fp_t)destroy_Meeting);
        OC_destroy_container(room_ptr->meetings);
        free(room_ptr);
	}
}

int get_Room_number(const struct Room* room_ptr)
{
    assert(room_ptr);
    return room_ptr->number;
}

int add_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr)
{
    void *const found_item_ptr = OC_find_item(room_ptr->meetings, meeting_ptr);

    if (found_item_ptr)
    {
        return FAILURE;
    }

    OC_insert(room_ptr->meetings, (void*)meeting_ptr);
    return SUCCESS;
}


struct Meeting* find_Room_Meeting(const struct Room* room_ptr, int time)
{
    assert(room_ptr);

    void *const item_ptr = OC_find_item_arg(room_ptr->meetings, &time,
                                            (OC_find_item_arg_fp_t)meeting_comp_to_time);

    if (!item_ptr)
    {
        return NULL;
    }

    return OC_get_data_ptr(item_ptr);
}

int remove_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr)
{
    assert(room_ptr);
    assert(meeting_ptr);

    void *const found_item_ptr = OC_find_item(room_ptr->meetings, meeting_ptr);
    if (!found_item_ptr)
    {
        return FAILURE;
    }

    OC_delete_item(room_ptr->meetings, found_item_ptr);
    return SUCCESS;
}

void clear_Room(struct Room* room_ptr)
{
    assert(room_ptr);
    OC_apply(room_ptr->meetings, (OC_apply_fp_t)destroy_Meeting);
    OC_clear(room_ptr->meetings);
    assert(OC_empty(room_ptr->meetings));
}

const struct Ordered_container* get_Room_Meetings(const struct Room* room_ptr)
{
    assert(room_ptr);
    return room_ptr->meetings;
}

void print_Room(const struct Room* room_ptr)
{
    assert(room_ptr);
    printf("--- Room %d ---\n", room_ptr->number);

    if (OC_empty(room_ptr->meetings))
    {
        printf("No meetings are scheduled\n");
    }
    else
    {
        OC_apply(room_ptr->meetings, (OC_apply_fp_t)print_Meeting);
    }
}

void save_Room(const struct Room* room_ptr, FILE* outfile)
{
    assert(room_ptr);
    assert(outfile);

    const int number_of_meetings = OC_get_size(room_ptr->meetings);
    fprintf(outfile, "%d %d\n", room_ptr->number, number_of_meetings);
    OC_apply_arg(room_ptr->meetings, (OC_apply_arg_fp_t)save_Meeting, outfile);
}

static int handle_load_room_error(const int bool_expr, FILE* input_file,
                                  struct Room** room_ptr)
{
    if (bool_expr)
    {
        discard_rest_of_input_line(input_file);
        destroy_Room(*room_ptr);
        *room_ptr = NULL;
    }
    return bool_expr;
}

struct Room* load_Room(FILE* infile, const struct Ordered_container* people)
{
    assert(infile);
    assert(people);

    int room_number = 0;
    int number_of_meetings = 0;

    struct Room* room_ptr = NULL;
    int return_val = fscanf(infile, "%d%d", &room_number, &number_of_meetings);

    if (!handle_load_room_error(return_val != 2, infile, &room_ptr))
    {
        room_ptr = create_Room(room_number);

        assert(number_of_meetings >= 0);
        for (int i = 0; i < number_of_meetings; ++i)
        {
            const struct Meeting* const meeting_ptr = load_Meeting(infile, people);

            if (handle_load_room_error(meeting_ptr == NULL, infile, &room_ptr))
            {
                break;
            }
            else
            {
                OC_insert(room_ptr->meetings, (void*)meeting_ptr);
            }
        }
    }

    return room_ptr;
}
