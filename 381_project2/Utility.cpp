#include "Utility.h"
#include <istream>
#include <cstring>

int convert_time_to_24_hour(int time) {
    if (time < k_EARLIEST_MEETING_TIME) {
        time += 12;
    }
    return time;
}

void discard_rest_of_line(std::istream& is) {
    while (is && is.get() != '\n') {
        ;
    }
}
