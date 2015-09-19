#include "Meeting.h"
#include "Ordered_container.h"
#include "Utility.h"

/* a Meeting contains a time, a topic, and a container of participants */
struct Meeting {
	char* topic;
	int time;
	struct Ordered_container* participants;	/* a container of pointers to struct Person objects */
};

int g_Meeting_memory = 0;

struct Meeting* create_Meeting(int time, const char* topic){

}