#include "Utility.h"

int convert_time_to_24_hour(int time) {
    if (time < k_EARLIEST_MEETING_TIME) {
        time += 12;
    }
    return time;
}

