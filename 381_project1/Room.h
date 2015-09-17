#ifndef ROOM_H
#define ROOM_H

/* 
Rooms are an opaque type containing a room number and a container of meetings.
*/

#include <stdio.h> /* for the declaration of FILE*/

/* incomplete declarations */
struct Room;
struct Meeting;
struct Ordered_container;

/* Create a Room object. 
This is the only function that allocates memory for a Room
and the contained data. */
struct Room* create_Room(int number);

/* Destroy a Room object.
This is the only function that frees the memory for a Room
and the contained data. The Meetings are destroyed before the
Meeting list is emptied. */
void destroy_Room(struct Room* room_ptr);

/* Return the room number. */
int get_Room_number(const struct Room* room_ptr);

/* Add the meeting to the room, return non-zero if a meeting already at that time, 0 if OK. */
int add_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr);

/* Return a pointer to the meeting at the specified time, NULL if not present. */
struct Meeting* find_Room_Meeting(const struct Room* room_ptr, int time);

/* Remove the supplied meeting from the room; return non-zero if not there; 0 if OK. 
The meeting is not destroyed because we may need to place it into another room. */
int remove_Room_Meeting(struct Room* room_ptr, const struct Meeting* meeting_ptr);

/* Clear and destroy the Meetings in a Room.
This function destroys each meeting and leaves the Room empty of meetings. */
void clear_Room(struct Room* room_ptr);

/* Return a pointer to the const container of meetings for read-only iterations over the meetings. */
const struct Ordered_container* get_Room_Meetings(const struct Room* room_ptr);

/* Print the data in a struct Room. */
void print_Room(const struct Room* room_ptr);

/* Write the room data to a file. */
void save_Room(const struct Room* room_ptr, FILE* outfile);

/* Read a room's data from a file stream, create the data object and 
return a pointer to it, NULL if invalid data discovered in file.
No check made for whether the room already exists or not. */
struct Room* load_Room(FILE* infile, const struct Ordered_container* people);

#endif
