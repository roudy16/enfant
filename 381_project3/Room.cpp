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

Room::Room(int room_number_) : m_room_number(room_number_)
{
}

Room::Room(std::ifstream& is, const People_list_t& people_list) {
    int number_of_meetings;
    is >> m_room_number >> number_of_meetings;
    if (!is.good() || m_room_number < 0) {
        throw Error("Invalid data found in file!");
    }

    // Allocate and load new Meetings from the file
    for (int i = 0; i < number_of_meetings; ++i) {
        Meeting* meeting_ptr = new Meeting(is, people_list, m_room_number);
        m_meetings[meeting_ptr->get_time()] = meeting_ptr;
    }
}

int Room::get_room_number() const {
    return m_room_number;
}

bool Room::has_Meetings() const {
    return !m_meetings.empty();
}
int Room::get_number_Meetings() const {
    return m_meetings.size();
}

void Room::add_meeting_check(int time) const {
    if (is_Meeting_present(time)) {
        throw Error("There is already a meeting at that time!");
    }
}

void Room::add_Meeting(int time, const string& topic) {
    add_meeting_check(time);
    m_meetings[time] = new Meeting(m_room_number, time, topic);
}

void Room::move_Meeting(int time, Meeting* old_meeting_ptr) {
    add_meeting_check(time);

    // Create a new Meeting object and add it to the Meeting container
    Meeting* new_meeting_ptr = new Meeting(m_room_number, time, *old_meeting_ptr);
    m_meetings[time] = new_meeting_ptr;

    // Have participants update their Commitments
    new_meeting_ptr->inform_participants_of_reschedule(old_meeting_ptr->get_time());
}

bool Room::is_Meeting_present(int time) const {
    auto iter = m_meetings.find(time);
    return iter != m_meetings.end();
}

Meeting* Room::get_Meeting(int time) {
    auto iter = m_meetings.find(time);
    if (iter == m_meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        return iter->second;
    }
}

const Meeting* Room::get_Meeting(int time) const {
    auto iter = m_meetings.find(time);
    if (iter == m_meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        return iter->second;
    }
}

void Room::deallocate_all_meetings(){
    for_each(m_meetings.begin(),
        m_meetings.end(),
        [](Meetings_t::iterator::value_type& pair){ delete pair.second; });
}

void Room::clear_Meetings() {
    deallocate_all_meetings();
    m_meetings.clear();
}

void Room::remove_Meeting(int time) {
    auto iter = m_meetings.find(time);
    if (iter == m_meetings.end()) {
        throw Error("No meeting at that time!");
    }
    else {
        delete iter->second;
        m_meetings.erase(iter);
    }
}


bool Room::is_participant_present(const Person* person_ptr) const {
    // Find first Meeting that the Person is a participant in
    auto meeting_iter = find_if(m_meetings.begin(),
        m_meetings.end(),
        [=](Meetings_t::const_iterator::value_type pair)->bool { 
            return pair.second->is_participant_present(person_ptr); 
        });

    // Return true of a Meeting was found that contains the Person
    return meeting_iter != m_meetings.end();
}

void Room::save(ostream& os) const {
    os << m_room_number << ' ' << m_meetings.size() << endl;

    for_each(m_meetings.begin(),
        m_meetings.end(),
        [&os](Meetings_t::const_iterator::value_type pair){ pair.second->save(os); });
}

bool Room::operator< (const Room& rhs) const {
    return m_room_number < rhs.m_room_number;
}

ostream& operator<< (ostream& os, const Room& room) {
    os << "--- Room " << room.m_room_number << " ---" << endl;

    if (room.get_number_Meetings() == 0) {
        os << "No meetings are scheduled" << endl;
    }
    else {
        // Have each Meeting output itself
        for_each(room.m_meetings.begin(),
            room.m_meetings.end(),
            [&os](Room::Meetings_t::const_iterator::value_type pair){ os << *(pair.second); });
    }

    return os;
}
