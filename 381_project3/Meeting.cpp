#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <string>

using namespace std;

Meeting::Meeting(ifstream& is, const Participants_t& people) {
    int number_of_participants;
    is >> m_time >> m_topic >> number_of_participants;
    if (!is.good()) {
        throw Error("Invalid data found in file!");
    }

    for (int i = 0; i < number_of_participants; ++i){
        string lastname;

        is >> lastname;
        if (!is.good()) {
            throw Error("Invalid data found in file!");
        }

        Person person(lastname);
        auto iter = people.find(&person);
        if (iter == people.end()) {
            throw Error("Invalid data found in file!");
        }
        else {
            participants.insert(*iter);
        }
    }
}

void Meeting::add_participant(const Person* p) {
    if (is_participant_present(p)) {
        throw Error("This person is already a participant!");
    }
    else {
        participants.insert(p);
    }
}

bool Meeting::is_participant_present(const Person* p) const {
    auto iter = participants.find(p);
    if (iter == participants.end()) {
        return false;
    }
    return true;
}

void Meeting::remove_participant(const Person* p) {
    if (!is_participant_present(p)) {
        throw Error("This person is not a participant in the meeting!");
    }
    else {
        participants.erase(participants.find(p));
    }
}

void Meeting::save(ostream& os) const {
    os << m_time << ' ' << m_topic << ' ' << participants.size() << '\n';

    for (auto p : participants) {
        os << p->get_lastname() << endl;
    }
}

bool Meeting::operator< (const Meeting& other) const {
    const int lhs = convert_time_to_24_hour(m_time);
    const int rhs = convert_time_to_24_hour(other.m_time);
    return lhs < rhs;
}

bool Meeting::operator== (const Meeting& other) const {
    return !(*this < other || other < *this);
}

ostream& operator<< (ostream& os, const Meeting& meeting) {
    os << "Meeting time: " << meeting.m_time << ", Topic: " << meeting.m_topic
       << "\nParticipants:";

    if (meeting.participants.size() == 0) {
        os << " None" << endl;
    }
    else {
        os << endl;
        for (auto p : meeting.participants) {
            os << *p << endl;
        }
    }
    return os;
}
