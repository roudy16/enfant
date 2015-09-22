#include "Meeting.h"
#include "Ordered_container.h"
#include "Utility.h"
#include "Person.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* a Meeting contains a time, a topic, and a container of participants */
struct Meeting {
	char* topic;
	int time;
	struct Ordered_container* participants;	/* a container of pointers to struct Person objects */
};

int g_Meeting_memory = 0;


struct Meeting* create_Meeting(int time, const char* topic){
    struct Meeting* meeting_ptr = malloc(sizeof(struct Meeting));

    if (meeting_ptr){
        meeting_ptr->topic = (char*)create_string(topic);
        meeting_ptr->time = time;
        meeting_ptr->participants = OC_create_container(&person_comp);
        ++g_Meeting_memory;
    }

    return meeting_ptr;
}

void destroy_Meeting(struct Meeting* meeting_ptr){
    if (meeting_ptr){
        OC_destroy_container(meeting_ptr->participants);
        free_string(meeting_ptr->topic);
        free(meeting_ptr);
        --g_Meeting_memory;
    }
}

int get_Meeting_time(const struct Meeting* meeting_ptr){
    if (meeting_ptr){
        return meeting_ptr->time;
    }
    else {
        return FAILURE;
    }
}

void set_Meeting_time(struct Meeting* meeting_ptr, int time){
    if (meeting_ptr){
        meeting_ptr->time = time;
    }
}

int add_Meeting_participant(struct Meeting* meeting_ptr,
                            const struct Person* person_ptr)
{
    assert(meeting_ptr);
    assert(person_ptr);

    void* const found_item_ptr = OC_find_item(meeting_ptr->participants,
                                              person_ptr);
    if (found_item_ptr){
        return FAILURE;
    }

    OC_insert(meeting_ptr->participants, (void*)person_ptr);
    return SUCCESS;
}

int remove_Meeting_participant(struct Meeting* meeting_ptr,
                               const struct Person* person_ptr)
{
    assert(meeting_ptr);
    assert(person_ptr);

    void* const found_item_ptr = OC_find_item(meeting_ptr->participants,
                                              person_ptr);
    if (!found_item_ptr){
        return FAILURE;
    }

    OC_delete_item(meeting_ptr->participants, found_item_ptr);
    return SUCCESS;
}

void print_Meeting(const struct Meeting* meeting_ptr){
    assert(meeting_ptr);

    printf("Meeting time: %d, Topic: %s\nParticipants:",
           meeting_ptr->time, meeting_ptr->topic);

    if (OC_empty(meeting_ptr->participants)){
        printf(" None\n");
        return;
    }

    printf("\n");
    OC_apply(meeting_ptr->participants, &print_Person);
}

static void print_person_lastname(const struct Person* person_ptr, FILE* outfile){
    assert(person_ptr);

    fprintf(outfile, "%s\n", get_Person_lastname(person_ptr));
}

void save_Meeting(const struct Meeting* meeting_ptr, FILE* outfile){
    assert(meeting_ptr);

    int number_of_participants = OC_get_size(meeting_ptr->participants);
    fprintf(outfile, "%d %s %d\n", meeting_ptr->time, meeting_ptr->topic,
           number_of_participants);
    OC_apply_arg(meeting_ptr->participants, &print_person_lastname, outfile);
    fflush(outfile); // Ensures data is written
}

static int person_to_name_comp(const char* const name_ptr,
                               const struct Person *const person_ptr)
{
    assert(person_ptr);
    assert(name_ptr);

    return strcmp(name_ptr, get_Person_lastname(person_ptr));
}

// Takes in a true/false expression that is true if an error has occured in the
// file reading. Handles errors, returns result of the true/false expression
static int handle_load_meeting_error(const int bool_expr, FILE* input_file,
                                     struct Meeting** meeting_ptr)
{
    if (bool_expr){
        discard_rest_of_input_line(input_file);
        destroy_Meeting(*meeting_ptr);
        *meeting_ptr = NULL;
    }
    return bool_expr;
}

struct Meeting* load_Meeting(FILE* input_file, const struct Ordered_container* people){
    assert(input_file);
    assert(people);

    static char string_buffer[MAX_INPUT + 1];
    int time = 0;
    int number_of_participants = 0;

    struct Meeting* meeting_ptr = NULL;
    int return_val = fscanf(input_file, "%d %" STRINGIFY_MACRO(MAX_INPUT) "s %d",
                            &time, string_buffer, &number_of_participants);


    if (!handle_load_meeting_error(return_val <= 0, input_file, &meeting_ptr)){
        meeting_ptr = create_Meeting(time, string_buffer); // buffer holds topic

        if (!meeting_ptr){
            return meeting_ptr;
        }

        for (int i = 0; i < number_of_participants; ++i){
            return_val = fscanf(input_file, "%" STRINGIFY_MACRO(MAX_INPUT) "s",
                                string_buffer);
            if (handle_load_meeting_error(return_val <= 0, input_file, &meeting_ptr)){
                break;
            }
            const void* item_ptr = OC_find_item_arg(people, string_buffer,
                                                    &person_to_name_comp);
            if (handle_load_meeting_error(!item_ptr, input_file, &meeting_ptr)){
                break;
            }
            OC_insert(meeting_ptr->participants, OC_get_data_ptr(item_ptr));
        }
    }

    return meeting_ptr;
}