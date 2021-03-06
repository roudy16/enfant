#ifndef MEETING_H
#define MEETING_H

#include <string>
#include <set>
#include "Utility.h"

/* Meeting class - this class represents a Meeting in terms of a time, topic, and 
list of participants. 
Note: Meeting's interface and I/O functions assume that the time is expressed in 12-hour format.

Public functions provide for maintaining the list of participants,
but no direct access to it is allowed. 

An overloaded operator< is provided to simplify constructing a list of Meetings 
in order by time, and an output operator to simplify printing the Meeting information.

We let the compiler supply the destructor and the copy/move constructors and assignment operators.
*/

// Forward declare Person class
class Person;

class Meeting {
public:
    using Participants_t = std::set<const Person*, Less_than_ptr<const Person*>>;

    // Destructor
    ~Meeting();
    // Construct a Meeting with provided location, time, and topic
    Meeting(int location_, int time_, const std::string& topic_);

    // Construct a Meeting from an input file stream in save format
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Meeting already exists or not.
    // Person list is needed to resolve references to meeting participants
    // Input for a member variable value is read directly into the member variable.
    Meeting(std::ifstream& is, const Participants_t& people, int location);

    // accessors
    int get_time() const;
    int get_location() const;
    const std::string& get_topic() const;

    // Meeting objects manage their own participant list. Participants
    // are identified by a pointer to that individual's Person object.

    // Add to the list, throw exception if participant was already there.
    void add_participant(const Person* p);
    // Return true if the person is a participant, false if not.
    bool is_participant_present(const Person* p) const;
    // Remove Person from the list, throw exception if participant was not found.
    void remove_participant(const Person* p);
    void remove_all_participants();

    // Move this Meeting's participants to the other Meeting, this Meeting
    // is left with no participants
    void move_participants_to(Meeting& other);

    // returns true if any of the meeting's participants have a commitment
    // at the passed in time.
    bool conflicts_exist(int time) const;

    // Informs participants that this Meeting has been moved from the old time.
    // The old time may be the same as th new time, this indicates a Room change.
    // This allows participants to update their commitments.
    void inform_participants_of_reschedule(int old_time) const;

    // Write a Meeting's data to a stream in save format with final endl.
    void save(std::ostream& os) const;

    // This operator defines the order relation between meetings, based just on the time
    bool operator< (const Meeting& other) const;

    friend std::ostream& operator<< (std::ostream&, const Meeting&);
private:
    // Default constructor
    Meeting() = delete;
    // Move constructor
    Meeting(Meeting&&) = delete;
    // Copy constructor
    Meeting(const Meeting&) = delete;
    // Move assignment
    Meeting& operator=(Meeting&&) = delete;
    // Copy assignment
    Meeting& operator=(const Meeting&) = delete;

    Participants_t m_participants;
    std::string m_topic;
    int m_location;
    int m_time;
};


// Print the Meeting data as follows:
// The meeting time and topic on one line, followed either by:
// the no-participants message with an endl
// or the partipcants, one per line, with an endl after each one
std::ostream& operator<< (std::ostream& os, const Meeting& meeting);

#endif // MEETING_H
