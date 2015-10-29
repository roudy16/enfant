#include "Utility.h"
#include <string>
#include <iostream>
#include <fstream>

using std::istream;
using std::ifstream;
using std::string;
using std::cin;

int convert_time_to_24_hour(int time) {
    if (time < k_EARLIEST_MEETING_TIME) {
        time += 12;
    }
    return time;
}

string read_string_from_stream(istream& is) {
    string temp;
    is >> temp;
    return temp;
}

void check_file_stream_status(ifstream& ifs) {
    if (!ifs.good()) {
        throw Error("Invalid data found in file!");
    }
}
