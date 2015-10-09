#ifndef ROOM_H
#define ROOM_H

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

/* *** NOTE: If after a function header is a comment "fill this in" remove the comment and replace
it with the proper code here in the header file.  All other functions should be defined
in the .cpp file. 
Comments starting with "***" are instructors to you - remove them from your finished code.
Remove this comment too. */

class Room {
public:
    // Construct a room with the specified room number and no meetings
    Room(int room_number_)
        /*fill this in*/
        // Construct a Room from an input file stream in save format, using the people list,
        // restoring all the Meeting information. 
        // Person list is needed to resolve references to meeting participants.
        // No check made for whether the Room already exists or not.
        // Throw Error exception if invalid data discovered in file.
        // Input for a member variable value is read directly into the member variable.
        Room(std::ifstream& is, const Ordered_list<const Person*, Less_than_ptr<const Person*> >& people_list);

    // Accessors
    int get_room_number() const
    {
        return m_room_number;
    }

    // Room objects manage their own Meeting container. Meetings are objects in
    // the container. The container of Meetings is not available to clients.

    // Add the Meeting, throw exception if there is already a Meeting at that time.
    // A copy of the supplied Meeting is stored in the Meeting container.
    void add_Meeting(const Meeting& m);
    // The supplied Meeting is moved into the Meeting container.
    void add_Meeting(Meeting&& m);
    // Return true if there is at least one meeting, false if none
    bool has_Meetings() const
    {/*fill this in*/
    }
    // Return the number of meetings in this room
    int get_number_Meetings() const
    {/*fill this in*/
    }
    // Return true if there is a Meeting at the time, false if not.
    bool is_Meeting_present(int time) const;
    // Return a reference if the Meeting is present, throw exception if not.
    Meeting& get_Meeting(int time);
    // Remove the specified Meeting, throw exception if a Meeting at that time was not found.
    void remove_Meeting(int time);
    // Remove and destroy all meetings
    void clear_Meetings()
    {/*fill this in*/
    }
    // Return true if the person is present in any of the meetings
    bool is_participant_present(const Person* person_ptr) const;

    // Write a Rooms's data to a stream in save format, with endl as specified.
    void save(std::ostream& os) const;

    // This operator defines the order relation between Rooms, based just on the number
    bool operator< (const Room& rhs) const
    {/*fill this in*/
    }

    /* *** provide a friend declaration for the output operator */

private:
    /* *** the meeting information must be kept in a container of Meeting objects */
    using Meetings_t = Ordered_list < Meeting > ;
    Meetings_t meetings;

    int m_room_number;

};

// Print the Room data as follows:
// The room heading with room number followed by an endl, followed by either:
// The no-meetings message with an endl, or
// The information for each meeting, which should automatically have a final endl
std::ostream& operator<< (std::ostream& os, const Room& room);

#endif // ROOM_H
