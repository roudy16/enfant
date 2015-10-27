#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <string>
#include <utility>

using namespace std;

Meeting::Meeting(Meeting&& original)
    : participants(move(original.participants)),
    m_topic(move(original.m_topic)),
    m_time(original.m_time)
{
}

Meeting::Meeting(int time_, Meeting&& original)
    : participants(move(original.participants)),
    m_topic(move(original.m_topic)),
    m_time(time_)
{
}

Meeting::Meeting(ifstream& is, const Participants_t& people) {
    int number_of_participants;
    is >> m_time >> m_topic >> number_of_participants;
    if (!is.good()) {
        throw Error("Invalid data found in file!");
    }

    // TODO step1
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

Meeting& Meeting::operator=(Meeting&& rhs) {
    participants = move(rhs.participants);
    m_topic = move(rhs.m_topic);
    m_time = move(rhs.m_time);

    return *this;
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

    // TODO step1
    for (auto p : participants) {
        os << p->get_lastname() << endl;
    }
}

bool Meeting::operator< (const Meeting& other) const {
    const int lhs = convert_time_to_24_hour(m_time);
    const int rhs = convert_time_to_24_hour(other.m_time);
    return lhs < rhs;
}

ostream& operator<< (ostream& os, const Meeting& meeting) {
    os << "Meeting time: " << meeting.m_time << ", Topic: " << meeting.m_topic
       << "\nParticipants:";

    if (meeting.participants.size() == 0) {
        os << " None" << endl;
    }
    else {
        os << endl;

        // TODO step1
        for (auto p : meeting.participants) {
            os << *p << endl;
        }
    }
    return os;
}
