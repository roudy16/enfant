#include "Meeting.h"
#include "Room.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <utility>
#include <algorithm>

using namespace std;

using People_list_t = std::set<const Person*, Less_than_ptr<const Person*>>;

bool Time_comp::operator()(int lhs, int rhs) const {
    lhs = convert_time_to_24_hour(lhs);
    rhs = convert_time_to_24_hour(rhs);
    return lhs < rhs;
}

Room::Room(std::ifstream& is, const People_list_t& people_list) {
    int number_of_meetings;
    is >> m_room_number >> number_of_meetings;
    if (!is.good() || m_room_number < 0) {
        throw LoadError();
    }

    // TODO step1
    for (int i = 0; i < number_of_meetings; ++i) {
        Meeting m(is, people_list, m_room_number);
        meetings[m.get_time()] = move(m);
    }
}

void Room::add_Meeting(Meeting&& m) {
    if (is_Meeting_present(m.get_time())) {
        throw Error("There is already a meeting at that time!");
    }
    else {
        meetings[m.get_time()] = move(m);
    }
}

bool Room::is_Meeting_present(int time) const {
    auto iter = meetings.find(time);
    return iter != meetings.end();
}

Meeting& Room::get_Meeting(int time) {
    auto iter = meetings.find(time);
    if (iter == meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        return iter->second;
    }
}

const Meeting& Room::get_Meeting(int time) const {
    auto iter = meetings.find(time);
    if (iter == meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        return iter->second;
    }
}

void Room::remove_Meeting(int time) {
    auto iter = meetings.find(time);
    if (iter == meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        meetings.erase(iter);
    }
}

bool Room::is_participant_present(const Person* person_ptr) const {
    bool return_val = false;
    // TODO step1
    for (auto& meeting : meetings) {
        if (meeting.second.is_participant_present(person_ptr)) {
            return_val = true;
            break;
        }
    }

    return return_val;
}

void Room::save(ostream& os) const {
    os << m_room_number << ' ' << meetings.size() << endl;

    // TODO step1
    for (auto& meeting : meetings) {
        meeting.second.save(os);
    }
}

ostream& operator<< (ostream& os, const Room& room) {
    os << "--- Room " << room.m_room_number << " ---" << endl;

    if (room.get_number_Meetings() == 0) {
        os << "No meetings are scheduled" << endl;
    }
    else {
        // TODO step1
        for (auto& meeting : room.meetings) {
            os << meeting.second;
        }
    }

    return os;
}
