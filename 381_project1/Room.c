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

static int meeting_comp(const struct Meeting *const meeting1_ptr,
                        const struct Meeting *const meeting2_ptr)
{
    int time1 = get_Meeting_time(meeting1_ptr);
    int time2 = get_Meeting_time(meeting2_ptr);

    // convert times to 24 hour range for comparison
    if (time1 < 9){
        time1 += 12;
    }
    if (time2 < 9){
        time2 += 12;
    }

    return time1 - time2;
}

struct Room* create_Room(int number){
    struct Room* new_room_ptr = malloc(sizeof(struct Room));
    if (!new_room_ptr){
        return new_room_ptr;
    }

    new_room_ptr->meetings = OC_create_container(&meeting_comp);
    if (!new_room_ptr->meetings){
        free(new_room_ptr);
        return NULL;
    }

    new_room_ptr->number = number;
    return new_room_ptr;
}

void destroy_Room(struct Room* room_ptr){
    assert(room_ptr);

    OC_apply(room_ptr->meetings, &destroy_Meeting);
    OC_destroy_container(room_ptr->meetings);
    free(room_ptr);
}

int get_Room_number(const struct Room* room_ptr){
    assert(room_ptr);
    return room_ptr->number;
}

int add_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr){
    void *const found_item_ptr = OC_find_item(room_ptr->meetings, meeting_ptr);

    if (found_item_ptr){
        return FAILURE;
    }

    OC_insert(room_ptr->meetings, (void*)meeting_ptr);
    return SUCCESS;
}

static int meeting_comp_to_time(const int* time_ptr, const struct Meeting* meeting_ptr){
    return *time_ptr - get_Meeting_time(meeting_ptr);
}

struct Meeting* find_Room_Meeting(const struct Room* room_ptr, int time){
    assert(room_ptr);
    void *const item_ptr = OC_find_item_arg(room_ptr->meetings, &time,
                                            &meeting_comp_to_time);

    if (!item_ptr){
        return NULL;
    }

    return OC_get_data_ptr(item_ptr);
}

int remove_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr){
    assert(room_ptr);
    assert(meeting_ptr);

    void *const found_item_ptr = OC_find_item(room_ptr->meetings, meeting_ptr);

    if (!found_item_ptr){
        return FAILURE;
    }

    OC_delete_item(room_ptr->meetings, found_item_ptr);
    return SUCCESS;
}

void clear_Room(struct Room* room_ptr){
    assert(room_ptr);
    OC_apply(room_ptr->meetings, &destroy_Meeting);
    assert(OC_empty(room_ptr->meetings));
}

const struct Ordered_container* get_Room_Meetings(const struct Room* room_ptr){
    assert(room_ptr);
    return room_ptr->meetings;
}

void print_Room(const struct Room* room_ptr){
    assert(room_ptr);
    printf("--- Room %d ---\n", room_ptr->number);

    if (OC_empty(room_ptr->meetings)){
        printf("No meetings are scheduled\n");
    }
    else {
        OC_apply(room_ptr->meetings, &print_Meeting);
    }
}

void save_Room(const struct Room* room_ptr, FILE* outfile){
    assert(room_ptr);
    assert(outfile);
    
    const int number_of_meetings = OC_get_size(room_ptr->meetings);
    fprintf(outfile, "%d %d", room_ptr->number, number_of_meetings);
    OC_apply_arg(room_ptr->meetings, &save_Meeting, outfile);
}