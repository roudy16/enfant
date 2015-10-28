#include "Person.h"
#include "Meeting.h"
#include "Utility.h"
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>

using namespace std;
using namespace std::placeholders;

Person::Person() {}

Person::~Person() {}

Person::Person(ifstream& is){
    is >> m_firstname;
    is >> m_lastname;
    is >> m_phoneno;
}

void Person::save(ostream& os) const {
    os << *this << endl;
}

bool Person::has_commitment_conflict(const Meeting* const meeting_ptr, int time) const {
    auto commitment_iter = find(m_commitments.begin(),
                                m_commitments.end(),
                                time);

    if (commitment_iter == m_commitments.end()) {
        return false;
    }

    return commitment_iter->mp_meeting != meeting_ptr;
}

void Person::add_commitment(const Meeting* meeting_ptr) const {
    if (has_commitment_conflict(meeting_ptr, meeting_ptr->get_time())) {
        throw Error("Person is already committed at that time!");
    }

    Commitment new_commitment(meeting_ptr);

    auto insert_iter = lower_bound(m_commitments.begin(),
                                   m_commitments.end(),
                                   new_commitment);


    m_commitments.insert(insert_iter, move(new_commitment));
    assert(verify_commitments_ordering());
}

void Person::add_commitment(Commitment&& original) const
{
    auto insert_iter = lower_bound(m_commitments.begin(),
                                   m_commitments.end(),
                                   original);

    m_commitments.insert(insert_iter, move(original));

    assert(verify_commitments_ordering());
}

Person::Commitments_t::iterator Person::find_commitment(int time) const {
    return find_if(m_commitments.begin(),
                   m_commitments.end(),
                   [=](const Commitment& c)->bool{ return c == time; });
}

void Person::remove_commitment(int time) const {
    auto commitment_iter = find_commitment(time);
    m_commitments.erase(commitment_iter);
    assert(verify_commitments_ordering());
}

static bool meeting_is_at_time(const Meeting* meeting_ptr, int time) {
    return meeting_ptr->get_time() == time;
}

void Person::change_commitment(int old_time, const Meeting* new_meeting_ptr) const {
    auto commitment_iter = find_commitment(old_time);

    assert(commitment_iter != m_commitments.end());

    // create a new commitment that has the updated room and time
    Commitment changed_commitment(new_meeting_ptr);

    // Remove the old commitment and insert the changed commitment
    m_commitments.erase(commitment_iter);
    add_commitment(move(changed_commitment));
}

void Person::clear_commitments() const {
    m_commitments.clear();
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

bool Person::verify_commitments_ordering() const {
    Commitments_t::const_iterator it = m_commitments.begin();
    if (it == m_commitments.end()) {
        return true;
    }

    bool is_ordered = true;
    for_each(m_commitments.begin(),
             m_commitments.end(),
             [&](const Commitment& c) {
                 if (!is_ordered) return;
                 if ((++it) == m_commitments.end()) return;
                 is_ordered = c < *it;
             });

    return is_ordered;
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
    return mp_meeting->get_location() < rhs.mp_meeting->get_location() ||
           (mp_meeting->get_location() == rhs.mp_meeting->get_location() && 
            mp_meeting->get_time() < rhs.mp_meeting->get_time());
}

bool Person::Commitment::operator== (int time) const {
    return mp_meeting->get_time() == time;
}

ostream& operator<< (ostream& os, const Person& person) {
    os << person.m_firstname << ' ' << person.m_lastname << ' ' << person.m_phoneno;
    return os;
}

