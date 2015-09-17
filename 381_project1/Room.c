/* skeleton file for Room.c
The struct declaration below must be used for Room objects.
Remove this comment and complete this file with all necessary code.
*/


/* a Room contains a container of meetings and a room number */
struct Room {
	struct Ordered_container* meetings;	/* a container of pointers to Meeting objects */
	int number;
};


