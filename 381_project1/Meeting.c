/* skeleton file for Meeting.c
The struct declaration below must be used for Meeting objects.
Remove this comment and complete this file with all necessary code.
*/


/* a Meeting contains a time, a topic, and a container of participants */
struct Meeting {
	char* topic;
	int time;
	struct Ordered_container* participants;	/* a container of pointers to struct Person objects */
};

