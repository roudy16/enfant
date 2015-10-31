#ifndef ROOM_H
#define ROOM_H

#include <set>
#include <map>
#include "Meeting.h"
#include "Utility.h"

/* A Room object contains a room number and a list containing Meeting objects stored with
meeting times as the key.  When created, a Room has no Meetings. When destroyed, the Meeting
objects in a Room are automatically destroyed.

Rooms manage the Meetings contained in them; functions are present for finding, adding,
or removing a Meeting specified by time.  The get_Meeting function returns a reference to the
specified Meeting, so that client code can modify the meeting - e.g. by adding a participant.
Note that modifying the time for a meeting in the container will disorder the meeting container, 
and so should not be attempted.

In addition, a Room can be asked to search for a particular Person being a participant 
in any of the Meetings in the Room. This makes it unnecessary for client code 
to be able to access the Meeting container in order to search for a specific participant.

We let the compiler supply the destructor and copy/move constructors and assignment operators.
*/ 

struct Time_comp {
    bool operator()(int lhs, int rhs) const;
};

class Room {
public:
    // Construct a room with the specified room number and no meetings
    Room(int room_number_);
    // Destructor
    ~Room();

    // Construct a Room from an input file stream in save format, using the people list,
    // restoring all the Meeting information. 
    // Person list is needed to resolve references to meeting participants.
    // No check made for whether the Room already exists or not.
    // Throw Error exception if invalid data discovered in file.
    // Input for a member variable value is read directly into the member variable.
    Room(std::ifstream& is, const std::set<const Person*, Less_than_ptr<const Person*>>& people_list);

    // Accessor
    int get_room_number() const;
    int get_number_Meetings() const;
    // Return a reference if the Meeting is present, throw exception if not.
    Meeting* get_Meeting(int time);
    const Meeting* get_Meeting(int time) const;

    // Room objects manage their own Meeting container. Meetings are objects in
    // the container. The container of Meetings is not available to clients.

    // Add the Meeting, throw exception if there is already a Meeting at that time.
    // This function does not allocate a new Meeting
    //void add_Meeting(Meeting* meeting_ptr);

    // Allocates a new meeting and adds it to the meetings container
    void add_Meeting(int time, const std::string& topic);
    // Allocates a new meeting and moves the old meetings participants to it.
    // old meeting is left with no participants.
    void move_Meeting(int time, Meeting* old_meeting_ptr);
    // Return true if there is at least one meeting, false if none
    bool has_Meetings() const;
    // Return the number of meetings in this room
    bool is_Meeting_present(int time) const;
    // Remove the specified Meeting, throw exception if a Meeting at that time was not found.
    void remove_Meeting(int time);
    // Remove and destroy all meetings
    void clear_Meetings();
    // Return true if the person is present in any of the meetings
    bool is_participant_present(const Person* person_ptr) const;

    // Write a Rooms's data to a stream in save format, with endl as specified.
    void save(std::ostream& os) const;

    // This operator defines the order relation between Rooms, based just on the number
    bool operator< (const Room& rhs) const;

    friend std::ostream& operator<< (std::ostream&, const Room&);

private:
    // Default constructor
    Room() = delete;
    // Move constructor
    Room(Room&&) = delete;
    // Copy constructor
    Room(const Room&) = delete;
    // Move assignment
    Room& operator=(Room&&) = delete;
    // Copy assignment
    Room& operator=(const Room&) = delete;

    using Meetings_t = std::map<int, Meeting*, Time_comp>;

    void deallocate_all_meetings();
    void add_meeting_check(int time) const;

    Meetings_t m_meetings;
    int m_room_number;
};

// Print the Room data as follows:
// The room heading with room number followed by an endl, followed by either:
// The no-meetings message with an endl, or
// The information for each meeting, which should automatically have a final endl
std::ostream& operator<< (std::ostream& os, const Room& room);

#endif // ROOM_H
