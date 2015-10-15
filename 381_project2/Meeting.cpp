#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <ostream>

using std::flush;
using std::endl;
using std::ostream;
using std::ifstream;

Meeting::Meeting(ifstream& is, const Participants_t& people) {
    int number_of_participants;
    is >> m_time >> m_topic >> number_of_participants;

    for (int i = 0; i < number_of_participants; ++i){
        String discard;
        String lastname;

        is >> discard >> lastname >> discard;

        Person person(lastname);
        auto iter = people.find(&person);
        if (iter == people.end()) {
            // TODO handle error
        }
        else {
            participants.insert(*iter);
        }
    }
}

void Meeting::add_participant(const Person* p) {
    if (is_participant_present(p)) {
        // TODO throw an error for this
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
        // TODO throw an error for this
    }
    else {
        participants.erase(participants.find(p));
    }
}

void Meeting::save(ostream& os) const {
    os << m_time << ' ' << m_topic << ' ' << participants.size() << '\n';

    for (auto p : participants) {
        os << *p << '\n';
    }

    os << flush;
}

bool Meeting::operator< (const Meeting& other) const {
    return m_time < other.m_time;
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
