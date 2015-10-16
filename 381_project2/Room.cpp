#include "Meeting.h"
#include "Room.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <utility>

using std::endl;
using std::ostream;
using std::ifstream;

using People_list_t = const Ordered_list < const Person*, Less_than_ptr<const Person*> > ;

Room::Room(std::ifstream& is, People_list_t& people_list) {
    int number_of_meetings;
    is >> m_room_number >> number_of_meetings;

    for (int i = 0; i < number_of_meetings; ++i) {
        Meeting m(is, people_list);
        meetings.insert(std::move(m));
    }
}

void Room::add_Meeting(const Meeting& m) {
    if (is_Meeting_present(m.get_time())) {
        throw Error("There is already a meeting at that time!");
    }
    else {
        meetings.insert(m);
    }
}

void Room::add_Meeting(Meeting&& m) {
    if (is_Meeting_present(m.get_time())) {
        throw Error("There is already a meeting at that time!");
    }
    else {
        meetings.insert(std::move(m));
    }
}

bool Room::is_Meeting_present(int time) const {
    auto iter = meetings.find(Meeting(time));
    return iter != meetings.end();
}

Meeting& Room::get_Meeting(int time) {
    auto iter = meetings.find(Meeting(time));
    if (iter == meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        return *iter;
    }
}

void Room::remove_Meeting(int time) {
    auto iter = meetings.find(Meeting(time));
    if (iter == meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        meetings.erase(iter);
    }
}

bool Room::is_participant_present(const Person* person_ptr) const {
    bool return_val = false;
    for (auto& meeting : meetings) {
        if (meeting.is_participant_present(person_ptr)) {
            return_val = true;
            break;
        }
    }

    return return_val;
}

void Room::save(ostream& os) const {
    os << m_room_number << ' ' << meetings.size() << endl;

    for (auto& meeting : meetings) {
        meeting.save(os);
    }
}

ostream& operator<< (ostream& os, const Room& room) {
    os << "--- Room " << room.m_room_number << " ---" << endl;

    if (room.get_number_Meetings() == 0) {
        os << "No meetings are scheduled" << endl;
    }
    else {
        for (auto& meeting : room.meetings) {
            os << meeting;
        }
    }

    return os;
}
