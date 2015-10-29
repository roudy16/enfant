#include "Utility.h"
#include <string>
#include <iostream>
#include <fstream>

using std::ifstream;
using std::string;
using std::cin;

int convert_time_to_24_hour(int time) {
    if (time < k_EARLIEST_MEETING_TIME) {
        time += 12;
    }
    return time;
}

std::string&& read_string_from_input() {
    string temp;
    cin >> temp;
    return move(temp);
}

void check_file_stream_status(ifstream& ifs) {
    if (!ifs.good()) {
        throw Error("Invalid data found in file!");
    }
}
