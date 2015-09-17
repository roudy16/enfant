#ifndef MEETING_H
#define MEETING_H

/* 
Meetings are an opaque type containing a time, a topic, and a container of participants.
*/

#include <stdio.h> /* for the declaration of FILE */

/* incomplete declarations */
struct Meeting;
struct Person;
struct Ordered_container;

/* global variable declarations for this module */
extern int g_Meeting_memory;			/* number of meeting structs currently allocated */


/* Create a Meeting object.
This is the only function that allocates memory for a Meeting
and the contained data. The supplied time is assumed to be in 12-hr form. */
struct Meeting* create_Meeting(int time, const char* topic);

/* Destroy a Meeting object.
This is the only function that frees the memory for a Meeting
and the contained data. It discards the participant list,
but of course does not delete the Persons themselves. */
void destroy_Meeting(struct Meeting* meeting_ptr);

/* Return and set the meeting time. The time value is assumed to be in 12-hr form.*/
int get_Meeting_time(const struct Meeting* meeting_ptr);
void set_Meeting_time(struct Meeting* meeting_ptr, int time);

/* Add a participant; return non-zero and do nothing if already present. */
int add_Meeting_participant(struct Meeting* meeting_ptr, const struct Person* person_ptr);

/* Return non-zero if the person is a participant, zero if not. */
int is_Meeting_participant_present(const struct Meeting* meeting_ptr, const struct Person * person_ptr);

/* Remove a participant; return non-zero if not present, zero if was present. */
int remove_Meeting_participant(struct Meeting* meeting_ptr, const struct Person* person_ptr);

/* Print the data in a struct Meeting. The time is expressed in 12-hr form with no AM/PM.*/
void print_Meeting(const struct Meeting* meeting_ptr);

/* Write the data in a Meeting to a file. The time is expressed in 12-hr form with no AM/PM.*/
void save_Meeting(const struct Meeting* meeting_ptr, FILE* outfile);

/* Read a Meeting's data from a file stream, create the data object and 
return a pointer to it, NULL if invalid data discovered in file.
No check made for whether the Meeting already exists or not. The time is expressed in 12-hr form with no AM/PM.*/
struct Meeting* load_Meeting(FILE* input_file, const struct Ordered_container* people);

#endif
