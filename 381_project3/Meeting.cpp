#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <string>
#include <utility>
#include <algorithm>
#include <functional>
#include <cassert>

using namespace std;
using namespace std::placeholders;

Meeting::Meeting()
{
}

Meeting::~Meeting() {
    // Remove all participants
    for_each(m_participants.begin(),
        m_participants.end(),
        bind(&Meeting::remove_participant, this, _1));
}

Meeting::Meeting(Meeting&& original)
    : m_participants(move(original.m_participants)),
    m_topic(move(original.m_topic)),
    m_location(original.m_location),
    m_time(original.m_time)
{
}

Meeting::Meeting(int location_, int time_)
    : m_location(location_), m_time(time_)
{
}

Meeting::Meeting(int location_, int time_, const std::string& topic_)
    : m_topic(topic_), m_location(location_), m_time(time_) 
{
}

Meeting::Meeting(int location_, int time_, Meeting&& original)
    : m_participants(move(original.m_participants)),
    m_topic(move(original.m_topic)),
    m_location(location_),
    m_time(time_)
{
}

Meeting::Meeting(ifstream& is, const Participants_t& people, int location) {
    int number_of_participants;
    is >> m_time >> m_topic >> number_of_participants;
    if (!is.good()) {
        throw LoadError();
    }

    m_location = location;

    // TODO step1
    for (int i = 0; i < number_of_participants; ++i){
        string lastname;

        is >> lastname;
        if (!is.good()) {
            throw LoadError();
        }

        Person person(lastname);
        auto iter = people.find(&person);
        if (iter == people.end()) {
            throw LoadError();
        }
        else {
            add_participant(*iter);
        }
    }
}

Meeting& Meeting::operator=(Meeting&& rhs) {
    m_participants = move(rhs.m_participants);
    m_topic = move(rhs.m_topic);
    m_location = move(rhs.m_location);
    m_time = move(rhs.m_time);

    return *this;
}

int Meeting::get_time() const {
    return m_time;
}

int Meeting::get_location() const {
    return m_location;
}

const std::string& Meeting::get_topic() const {
    return m_topic;
}

void Meeting::add_participant(const Person* p) {
    assert(!is_participant_present(p));

    p->add_commitment(this);
    m_participants.insert(p);
}

bool Meeting::is_participant_present(const Person* p) const {
    auto iter = m_participants.find(p);
    if (iter == m_participants.end()) {
        return false;
    }
    return true;
}

void Meeting::remove_participant(const Person* p) {
    if (!is_participant_present(p)) {
        throw Error("This person is not a participant in the meeting!");
    }
    else {
        // have Person remove this commitment then remove the Person
        // from the list of participants
        p->remove_commitment(m_time);
        m_participants.erase(m_participants.find(p));
    }
}

bool Meeting::conflicts_exist(int time) const {
    auto person_iter = find_if(m_participants.begin(),
        m_participants.end(),
        [=](const Person* p)->bool{ return p->has_commitment_conflict(this, time); });

    return person_iter != m_participants.end();
}



void Meeting::inform_participants_of_reschedule(int old_time) const 
{
    for_each(m_participants.begin(),
        m_participants.end(),
        [=](const Person* p){
            p->change_commitment(old_time, this);
        });
}

void Meeting::save(ostream& os) const {
    os << m_time << ' ' << m_topic << ' ' << m_participants.size() << '\n';

    // TODO step1
    for (auto p : m_participants) {
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

    if (meeting.m_participants.size() == 0) {
        os << " None" << endl;
    }
    else {
        os << endl;

        // TODO step1
        for (auto p : meeting.m_participants) {
            os << *p << endl;
        }
    }
    return os;
}
