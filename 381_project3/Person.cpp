#include "Person.h"
#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;

Person::Person(const std::string& lastname_) : m_lastname(lastname_) {}

Person::Person(const std::string& firstname_, const std::string& lastname_, const std::string& phoneno_)
    : m_firstname(firstname_), m_lastname(lastname_), m_phoneno(phoneno_) {}

Person::Person(ifstream& is){
    is >> m_firstname;
    is >> m_lastname;
    is >> m_phoneno;
}

std::string Person::get_lastname() const {
    return m_lastname;
}

void Person::save(ostream& os) const {
    os << *this << endl;
}

bool Person::verify_commitments_ordering() const {
#ifdef DEBUG
    // Lists with one or fewer items are always in order
    if (m_commitments.size() < 2) {
        return true;
    }

    // Compare all Commitments in the list to ensure proper ordering.
    auto it = m_commitments.begin();
    bool is_ordered = true;
    for_each(m_commitments.begin(),
        m_commitments.end(),
        [&](const Commitment& c) {
            if (!is_ordered) return;
            // increment it so that it points to the item in the list 
            // ahead of c, return if it points to end()
            if ((++it) == m_commitments.end()) return;
            is_ordered = c < *it;
        });

    return is_ordered;
#else
    return true;
#endif
}

bool Person::has_commitment_conflict(const Meeting* const meeting_ptr, int time) const {
    // Attempt to find a Commitment that conflicts with passed in meeting and time
    auto commitment_iter = find_if(m_commitments.begin(),
                                   m_commitments.end(),
                                   [=](const Commitment& c)->bool{
                                       return c.mp_meeting != meeting_ptr &&
                                              c.mp_meeting->get_time() == time;
                                   });

    // return true if a conflict was found
    return commitment_iter != m_commitments.end();
}

bool Person::has_commitments() const {
    return !m_commitments.empty();
}

void Person::add_commitment(const Meeting* meeting_ptr) const {
    if (has_commitment_conflict(meeting_ptr, meeting_ptr->get_time())) {
        throw Error("Person is already committed at that time!");
    }

    Commitment new_commitment(meeting_ptr);

    // Find the first Commitment that compares not less than new_commitment using
    // the Commitment::operator<
    auto insert_iter = find_if(m_commitments.begin(),
        m_commitments.end(),
        [&](const Commitment& c)->bool{return !(c < new_commitment); });

    m_commitments.insert(insert_iter, new_commitment);
    assert(verify_commitments_ordering());
}

Person::Commitments_t::iterator Person::find_commitment(int time) const {
    // Find the Commitment that matches the time if it exists
    return find_if(m_commitments.begin(),
                   m_commitments.end(),
                   [=](const Commitment& c)->bool{ return c.mp_meeting->get_time() == time; });
}

void Person::remove_commitment(int time) const {
    // Find the Commitment that matches the time then remove it
    auto commitment_iter = find_commitment(time);
    assert(commitment_iter != m_commitments.end());

    m_commitments.erase(commitment_iter);
    assert(verify_commitments_ordering());
}

void Person::change_commitment(int old_time, const Meeting* new_meeting_ptr) const {
    // Remove the old commitment and insert the changed commitment
    remove_commitment(old_time);
    add_commitment(new_meeting_ptr);
}

void Person::print_commitments() const {
    assert(verify_commitments_ordering());

    if (m_commitments.empty()) {
        cout << "No commitments" << endl;
        return;
    }

    // Let each commitment print itself
    for_each(m_commitments.begin(),
             m_commitments.end(),
             [](const Commitment& c){ c.print(); });
}

bool Person::operator< (const Person& rhs) const {
    return m_lastname < rhs.m_lastname;
}

Person::Commitment::Commitment(const Meeting* new_meeting_ptr)
    : mp_meeting(new_meeting_ptr)
{
}

void Person::Commitment::print() const {
    cout << "Room:" << mp_meeting->get_location()
         << " Time: " << mp_meeting->get_time()
         << " Topic: " << mp_meeting->get_topic() << endl;
}

bool Person::Commitment::operator< (const Commitment& rhs) const {
    // Commitments are ordered first by room number then by time
    // of the associated Meeting
    return mp_meeting->get_location() < rhs.mp_meeting->get_location() ||
           (mp_meeting->get_location() == rhs.mp_meeting->get_location() && 
            *mp_meeting < *rhs.mp_meeting);
}

ostream& operator<< (ostream& os, const Person& person) {
    // Set up string to be output then copy it to cout via std::copy
    // and an ostream_iterator.

    // use of += avoids extra string copying
    string person_output = person.m_firstname;
    person_output += ' ';
    person_output += person.m_lastname;
    person_output += ' ';
    person_output += person.m_phoneno;

    copy(person_output.begin(), person_output.end(), ostream_iterator<char>(os));
    return os;
}
