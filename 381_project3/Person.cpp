#include "Person.h"
#include "Utility.h"
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>

using namespace std;

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

bool Person::has_commitment(int time) const {
    auto commitment_iter = find(m_commitments.begin(),
                                m_commitments.end(),
                                time);

    return commitment_iter != m_commitments.end();
}

void Person::add_commitment(int new_room_number, int new_time, const string& topic) {
    if (has_commitment(new_time)) {
        throw Error("Person is already committed at that time!");
    }

    Commitment new_commitment(topic, new_room_number, new_time);

    auto insert_iter = lower_bound(m_commitments.begin(),
                                   m_commitments.end(),
                                   new_commitment);


    m_commitments.insert(insert_iter, move(new_commitment));
    assert(verify_commitments_ordering());
}

void Person::add_commitment(Commitment&& original) 
{
    auto insert_iter = lower_bound(m_commitments.begin(),
                                   m_commitments.end(),
                                   original);

    m_commitments.insert(insert_iter, move(original));

    assert(verify_commitments_ordering());
}

void Person::remove_commitment(int time) {
    auto commitment_iter = find(m_commitments.begin(),
                                m_commitments.end(),
                                time);

    m_commitments.erase(commitment_iter);
    assert(verify_commitments_ordering());
}

void Person::change_commitment(int old_time, int new_room_number, int new_time) {
    auto commitment_iter = find(m_commitments.begin(),
                                m_commitments.end(),
                                old_time);

    assert(commitment_iter != m_commitments.end());

    // create a new commitment that has the updated room and time
    Commitment& old_commitment(*commitment_iter);
    Commitment changed_commitment(old_commitment.m_topic,
                                  old_commitment.m_room_number,
                                  old_commitment.m_time);

    // Remove the old commitment and insert the changed commitment
    m_commitments.erase(commitment_iter);
    add_commitment(move(changed_commitment));
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

Person::Commitment::Commitment(const string& topic, int room_number, int time)
    : m_topic(topic), m_room_number(room_number), m_time(time)
{
}

void Person::Commitment::print() const {
    cout << "Room:" << m_room_number
         << " Time: " << m_time
         << " Topic: " << m_topic << endl;
}

bool Person::Commitment::operator< (const Commitment& rhs) const {
    return m_room_number < rhs.m_room_number &&
           m_time < rhs.m_time;
}

bool Person::Commitment::operator== (int time) const {
    return m_time == time;
}

ostream& operator<< (ostream& os, const Person& person) {
    os << person.m_firstname << ' ' << person.m_lastname << ' ' << person.m_phoneno;
    return os;
}

