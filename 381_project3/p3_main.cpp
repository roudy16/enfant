#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <functional>
#include <limits>
#include <iostream>
#include <fstream>
#include <iterator>
#include <new>
#include <cassert>

using namespace std;

// Type aliases for containers of Rooms and Persons 
using Rooms_t = vector<Room*>;
using People_t = set<const Person*, Less_than_ptr<const Person*>>;

// struct that holds all active Rooms and Persons
struct Schedule {
    Rooms_t m_rooms;
    People_t m_people;
};


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// Initialize a table of function pointers that correspond to input commands 
static void init_command_table(map<string, void(*)(Schedule&)> &commands);

// Find a room in the schedule by number if it exists, throw an Error
// if no such Room is found
static Room& find_room(Schedule& schedule, const int room_number);

// Find a room in the schedule by number if it exists
static Rooms_t::iterator find_room_iter(Schedule& schedule, const int room_number);

// Perform binary search for a Room with same room number
static Rooms_t::iterator find_room_iter_helper(Schedule& schedule,
                                               const int room_number);

// Find a meeting in the schedule by number if it exists, throw an Error
// if no such Meeting is found
//static Meeting& find_meeting(Schedule& schedule, const int room_number, const int meeting_time);

// Find a person in the schedule by lastname if it exists, throw an Error
// if no such Person is found
static const Person& find_person(Schedule& schedule, const string& lastname);

// Find a person in the schedule by lastname if it exists.
static People_t::iterator find_person_iter(Schedule& schedule, const string& lastname);

// Returns true if a Room with matching room number exists in the Schedule
static bool is_room_present(Schedule& schedule, const int room_number);

// ignores characters until a newline character '\n' is read, then the beginning of
// the stream is set to just after the '\n'
static void discard_rest_of_line(std::istream& is);

// Validate value ranges according to project specification, returns
// non-zero if argument is valid, returns zero otherwise.
static bool is_room_range_valid(const int room_number);
static bool is_time_range_valid(int time);

// Returns an int read from the stream, throws an error if reading fails
static int read_int_from_stream(std::istream& is);

// Reads and checks validity of time read from stream, throws error if reading
// fails or value is invalid
static int read_time_from_stream(std::istream& is);

// Reads and checks validity of room number read from stream, throws error if
// reading fails or value is invalid
static int read_room_number_from_stream(std::istream& is);

static Rooms_t::iterator get_room_from_input(Schedule& schedule);

// Throw an Error if Person is in a participant in a Meeting within the Room
static void detect_room_participant_helper(Room* room_ptr, const Person* person_ptr);

// Assists in adding a new Room, does not check if room already exists
static void add_room_helper(Schedule& schedule, Room* const room_ptr);

// TODO
static void clear_all_meetings(Schedule& schedule);
static void clear_all_rooms(Schedule& schedule);
static void clear_all_people(Schedule& schedule);
static void deallocate_all(Schedule& schedule);


/* ##################### */
/*  COMMANDS FROM SPEC   */
/* ##################### */


static void print_person_command(Schedule& schedule);
static void print_room_command(Schedule& schedule);
static void print_meeting_command(Schedule& schedule);
static void print_all_meetings_command(Schedule& schedule);
static void print_all_people_command(Schedule& schedule);
static void print_commitments_command(Schedule& schedule);
static void print_memory_allocations_command(Schedule& schedule);
static void add_to_people_list_command(Schedule& schedule);
static void add_room_command(Schedule& schedule);
static void add_meeting_command(Schedule& schedule);
static void add_person_to_meeting_in_room_command(Schedule& schedule);
static void reschedule_meeting_command(Schedule& schedule);
static void delete_individual_command(Schedule& schedule);
static void delete_room_command(Schedule& schedule);
static void delete_meeting_command(Schedule& schedule);
static void delete_participant_command(Schedule& schedule);
static void delete_all_individual_command(Schedule& schedule);
static void delete_schedule_command(Schedule& schedule);
static void deallocate_all_command(Schedule& schedule);
static void save_data_command(Schedule& schedule);
static void load_data_command(Schedule& schedule);


/* #################### */
/* FUNCTION DEFINITIONS */
/* #################### */


static void init_command_table(map<string, void(*)(Schedule&)> &commands) {
    commands["pi"] = &print_person_command;
    commands["pr"] = &print_room_command;
    commands["pm"] = &print_meeting_command;
    commands["ps"] = &print_all_meetings_command;
    commands["pg"] = &print_all_people_command;
    commands["pc"] = &print_commitments_command;
    commands["pa"] = &print_memory_allocations_command;

    commands["ai"] = &add_to_people_list_command;
    commands["ar"] = &add_room_command;
    commands["am"] = &add_meeting_command;
    commands["ap"] = &add_person_to_meeting_in_room_command;

    commands["di"] = &delete_individual_command;
    commands["dr"] = &delete_room_command;
    commands["dm"] = &delete_meeting_command;
    commands["dp"] = &delete_participant_command;
    commands["ds"] = &delete_schedule_command;
    commands["dg"] = &delete_all_individual_command;
    commands["da"] = &deallocate_all_command;

    commands["rm"] = &reschedule_meeting_command;
    commands["sd"] = &save_data_command;
    commands["ld"] = &load_data_command;
}

static void discard_rest_of_line(std::istream& is) {
    is.clear();
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    // Create table of function pointers that map to an input command
    map<string, void (*)(Schedule&)> commands;
    init_command_table(commands);

    Schedule schedule;
    string command_input;
    command_input.resize(2);

    while (true) {
        try {
            cout << "\nEnter command: ";

            // read two characters while skipping whitespace then find 
            // associated function pointer if it exists.
            cin >> command_input[0] >> command_input[1];
            auto command_iter = commands.find(command_input);

            // If function pointer not found check to see if quit command was
            // entered, throw unrecognized command Error otherwise.
            if (command_iter == commands.end()) {
                if (command_input.compare(0, 2, "qq") == 0) {
                    break; // Break out of while loop
                }
                else {
                    throw Error("Unrecognized command!");
                }
            }

            // Call function associated with entered command
            command_iter->second(schedule);
        }
        catch (Error& e) {
            discard_rest_of_line(cin);
            cout << e.msg << endl;
        }
        catch (LoadError& le) {
            cout << "Invalid data found in file!" << endl;
        }
        catch (std::bad_alloc& e){
            // TODO handle bad allocs, look at spec for what to do
            cout << e.what() << endl;
        } 
        catch (...) {
            // catch all for stuff thrown from not my code
        }
    } // End while loop

    deallocate_all_command(schedule);
    cout << "Done" << endl;
    return 0;
}

static bool is_room_range_valid(const int room_number) {
    return room_number > 0;
}

static bool is_time_range_valid(int time) {
    time = convert_time_to_24_hour(time);
    return time >= k_EARLIEST_MEETING_TIME && time <= k_LATEST_MEETING_TIME;
}

static int read_int_from_stream(std::istream& is) {
    int val;
    is >> val;

    if (!is.good()) {
        throw Error("Could not read an integer value!");
    }

    return val;
}

static int read_time_from_stream(std::istream& is) {
    int time = read_int_from_stream(is);

    if (!is_time_range_valid(time)) {
        throw Error("Time is not in range!");
    }

    return time;
}

static int read_room_number_from_stream(std::istream& is) {
    int room_number = read_int_from_stream(is);

    if (!is_room_range_valid(room_number)) {
        throw Error("Room number is not in range!");
    }

    return room_number;
}

static void print_person_command(Schedule& schedule) {
    string lastname;
    cin >> lastname;

    const Person& person = find_person(schedule, lastname);
    cout << person << endl;
}

static void print_room_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);
    const Room& room = find_room(schedule, room_number);
    cout << room;
}

static void print_meeting_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);
    const Room& room = find_room(schedule, room_number);
    int meeting_time = read_time_from_stream(cin);
    const Meeting* meeting = room.get_Meeting(meeting_time);
    cout << *meeting;
}

static void print_all_meetings_command(Schedule& schedule) {
    if (schedule.m_rooms.empty()) {
        cout << "List of rooms is empty" << endl;
    }
    else {
        cout << "Information for " << schedule.m_rooms.size() << " rooms:" << endl;
         // TODO step1
        for (auto room_p : schedule.m_rooms) {
            cout << *room_p;
        }
    }
}

static void print_all_people_command(Schedule& schedule) {
    if (schedule.m_people.empty()) {
        cout << "List of people is empty" << endl;
    }
    else {
        cout << "Information for " << schedule.m_people.size() << " people:" << endl;
        for (auto person_p : schedule.m_people) {
            cout << *person_p << endl;
        }
    }
}

static void print_commitments_command(Schedule& schedule) {
    string lastname;
    cin >> lastname;

    const Person& person = find_person(schedule, lastname);
    person.print_commitments();
}

static void print_memory_allocations_command(Schedule& schedule) {
    int total_meetings = 0;

    // Count total number of meetings in the schedule
    for_each(schedule.m_rooms.begin(),
        schedule.m_rooms.end(),
        [&total_meetings](Room* rm){ total_meetings += rm->get_number_Meetings(); });

    cout << "Memory allocations:" << '\n';
    cout << "Persons: " << schedule.m_people.size() << '\n';
    cout << "Meetings: " << total_meetings << '\n';
    cout << "Rooms: " << schedule.m_rooms.size() << '\n';
}


static void add_to_people_list_command(Schedule& schedule){
    string firstname, lastname, phoneno;
    cin >> firstname >> lastname >> phoneno;

    // Ensure the person does not already exist
    auto iter = find_person_iter(schedule, lastname);
    if (iter != schedule.m_people.end()) {
        throw Error("There is already a person with this last name!");
    }

    // Add new Person to people list
    schedule.m_people.insert(new Person(firstname, lastname, phoneno));
    cout << "Person " << lastname << " added" << endl;
}

// Assists in adding a new Room, does not check if room already exists
static void add_room_helper(Schedule& schedule, Room* const room_ptr) {
    auto room_iter = find_room_iter_helper(schedule, room_ptr->get_room_number());

    schedule.m_rooms.insert(room_iter, room_ptr);
}

static bool is_room_present(Schedule& schedule, const int room_number) {
    // Find the first Room with a number that is not less than room_number
    auto room_iter = find_room_iter_helper(schedule, room_number);

    // Return true if Room with room_number was found
    return room_iter != schedule.m_rooms.end() &&
        (*room_iter)->get_room_number() == room_number;
}

static void add_room_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);

    // Ensure that the room does not already exist
    if (is_room_present(schedule, room_number)) {
        throw Error("There is already a room with this number!");
    }

    // Add new Room to rooms list
    add_room_helper(schedule, new Room(room_number));
    cout << "Room " << room_number << " added" << endl;
}

static void add_meeting_command(Schedule& schedule) {
    // Read and check room number for validity
    int room_number = read_room_number_from_stream(cin);

    Room& room = find_room(schedule, room_number);
    int meeting_time = read_time_from_stream(cin);

    string topic;
    cin >> topic;

    room.add_Meeting(meeting_time, topic);

    cout << "Meeting added at " << meeting_time << endl;
}

static void add_person_to_meeting_in_room_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);
    Room& room = find_room(schedule, room_number);

    int meeting_time = read_time_from_stream(cin);
    Meeting* meeting = room.get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    const Person& person = find_person(schedule, lastname);

    if (meeting->is_participant_present(&person)) {
        throw Error("This person is already a participant!");
    }

    // Add participant to meeting
    meeting->add_participant(&person);
    cout << "Participant " << lastname << " added" << endl;
}

static void reschedule_meeting_command(Schedule& schedule) {
    /* we want to find the old room and meeting then find the new room.
       if any errors occur during this process the called functions will
       throw an appropriate exception 
    */

    // Find room of meeting we want to reschedule
    int old_room_number = read_room_number_from_stream(cin);
    Room& old_room = find_room(schedule, old_room_number);

    // Find meeting we want to reschedule
    int old_meeting_time = read_time_from_stream(cin);
    Meeting* old_meeting = old_room.get_Meeting(old_meeting_time);

    // Find room we want to move the meeting to
    int new_room_number = read_room_number_from_stream(cin);
    Room& new_room = find_room(schedule, new_room_number);

    // Ensure no meeting is already scheduled at that time in the new room
    // and check to the case where no change is needed
    int new_meeting_time = read_time_from_stream(cin);
    if (old_room_number == new_room_number && old_meeting_time == new_meeting_time) {
        cout << "No change made to schedule" << endl;
        return;
    }
    if (new_room.is_Meeting_present(new_meeting_time)) {
        throw Error("There is already a meeting at that time!");
    }

    // Check to see if there are any commitment conflicts
    if (old_meeting->conflicts_exist(new_meeting_time)) {
        throw Error("A participant is already committed at the new time!");
    }

    // At this point it is safe to create the new_meeting in the new room by
    // moving the contents of the old meeting but changing the meeting time.
    //Meeting new_meeting(new_room_number, new_meeting_time, move(old_meeting));
    new_room.move_Meeting(new_meeting_time, old_meeting);

    // We inform the participants of the reschedule so they update their commitments
    new_room.get_Meeting(new_meeting_time)->inform_participants_of_reschedule(old_meeting_time);

    // Remove the old Meeting object from the Room
    old_room.remove_Meeting(old_meeting_time);

    cout << "Meeting rescheduled to room " << new_room_number
         << " at " << new_meeting_time << endl;
}

// Throw Error if Person is in a Meeting in the Room
static void detect_room_participant_helper(Room* room_ptr, const Person* person_ptr) {
    if (room_ptr->is_participant_present(person_ptr)) {
        throw Error("This person is a participant in a meeting!");
    }
}

static void delete_individual(Schedule& schedule, const string& lastname) {
    // Make sure the person exists
    auto person_iter = find_person_iter(schedule, lastname);

    // If the person is scheduled for a meeting we cannot delete them, check
    // to see if the person is in any meetings
    for_each(schedule.m_rooms.begin(),
        schedule.m_rooms.end(),
        bind(detect_room_participant_helper, std::placeholders::_1, *person_iter));

    // Free memory allocated for Person object before erasing node
    delete *person_iter;
    schedule.m_people.erase(person_iter);
}

static void delete_individual_command(Schedule& schedule){
    string lastname;
    cin >> lastname;

    delete_individual(schedule, lastname);

    cout << "Person " << lastname << " deleted" << endl;
}

static Rooms_t::iterator get_room_from_input(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);
    return find_room_iter(schedule, room_number);
}

static void delete_room_command(Schedule& schedule){
    auto room_iter = get_room_from_input(schedule);
    int room_number = (*room_iter)->get_room_number();

    // Free memory allocated for Room object before erasing node
    delete *room_iter;
    schedule.m_rooms.erase(room_iter);
    cout << "Room " << room_number << " deleted" << endl;
}

static void delete_meeting_command(Schedule& schedule){
    auto room_iter = get_room_from_input(schedule);

    int meeting_time = read_time_from_stream(cin);

    (*room_iter)->remove_Meeting(meeting_time);
    cout << "Meeting at " << meeting_time << " deleted" << endl;
}

static void delete_participant_command(Schedule& schedule){
    auto room_iter = get_room_from_input(schedule);
    int meeting_time = read_time_from_stream(cin);

    Meeting* meeting = (*room_iter)->get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    auto person_iter = find_person_iter(schedule, lastname);

    meeting->remove_participant(*person_iter);
    cout << "Participant " << lastname << " deleted" << endl;
}

static int number_of_meetings(Schedule& schedule) {
    int num_meetings = 0;
    // Check to see if any meetings exist in any room
    for_each(schedule.m_rooms.begin(),
             schedule.m_rooms.end(),
             [&num_meetings](const Room* room){ num_meetings += room->get_number_Meetings(); }
             ); // end of for_each arg list

    return num_meetings;
}

static void delete_all_individual_command(Schedule& schedule){
    if (number_of_meetings(schedule) > 0)
    {
        throw Error("Cannot clear people list unless there are no meetings!");
    }

    // deallocate all Person objects in the people list
    clear_all_people(schedule);
    cout << "All persons deleted" << endl;
}

static void deallocate_all_command(Schedule& schedule) {
    deallocate_all(schedule);

    cout << "All rooms and meetings deleted\n"
         << "All persons deleted" << endl;
}

static void delete_schedule_command(Schedule& schedule){
    clear_all_meetings(schedule);
    cout << "All meetings deleted" << endl;
}

static void clear_all_meetings(Schedule& schedule) {
    for_each(schedule.m_rooms.begin(),
             schedule.m_rooms.end(),
             [](Room* rm){ rm->clear_Meetings(); } );
}

static void clear_all_rooms(Schedule& schedule) {
    // the one range for
    for (auto room_ptr : schedule.m_rooms) {
        delete room_ptr;
    }
    schedule.m_rooms.clear();
}

static void clear_all_people(Schedule& schedule) {
    for_each(schedule.m_people.begin(),
             schedule.m_people.end(),
             [](const Person* p){ delete p; });

    schedule.m_people.clear();
}

static void deallocate_all(Schedule& schedule){
    clear_all_meetings(schedule);
    clear_all_rooms(schedule);
    clear_all_people(schedule);
}

static void save_data_command(Schedule& schedule){
    string filename;
    cin >> filename;

    std::ofstream ofs(filename.c_str());
    if (!ofs.good()) {
        throw Error("Could not open file!");
    }

    ofs << schedule.m_people.size() << endl;
    for (auto person_ptr : schedule.m_people) {
        person_ptr->save(ofs);
    }

    ofs << schedule.m_rooms.size() << endl;
    for (auto room_ptr : schedule.m_rooms) {
        room_ptr->save(ofs);
    }

    cout << "Data saved" << endl;
}

static void load_data_command(Schedule& schedule){
    string filename;
    cin >> filename;

    std::ifstream ifs(filename.c_str());
    if (!ifs.good()) {
        throw Error("Could not open file!");
    }

    // Save old contents in case original state needs to be restored
    Schedule old_schedule;
    old_schedule = move(schedule);
    schedule.m_people.clear();
    schedule.m_rooms.clear();

    try {
        int number_of_people;
        ifs >> number_of_people;
        if (!ifs.good()) {
            throw Error("Invalid data found in file!");
        }

        for (int i = 0; i < number_of_people; ++i) {
            schedule.m_people.insert(new Person(ifs));
        }

        int number_of_rooms;
        ifs >> number_of_rooms;
        if (!ifs.good()) {
            throw Error("Invalid data found in file!");
        }

        for (int i = 0; i < number_of_rooms; ++i) {
            add_room_helper(schedule, new Room(ifs, schedule.m_people));
        }

        cout << "Data loaded" << endl;
        deallocate_all(old_schedule);
    }
    catch (...) {
        // Deallocate any objects created before Error thrown then move old data
        // back to the schedule to leave schedule in same state as before. Ensure
        // old_schedule is cleared.
        deallocate_all(schedule);
        schedule = move(old_schedule);

        throw;
    }
}

static Room& find_room(Schedule& schedule, const int room_number) {
    auto room_iter = find_room_iter(schedule, room_number);
    if (room_iter == schedule.m_rooms.end()) {
        throw Error("No room with that number!");
    }

    return **room_iter;
}

static Rooms_t::iterator find_room_iter_helper(Schedule& schedule,
                                               const int room_number)
{
    // Create a comparator and a probe Room to use to try to find a Room
    // with the same number as the room number passed in
    Less_than_ptr<Room*> comp;
    Room probe_room(room_number);
    return lower_bound(schedule.m_rooms.begin(),
                       schedule.m_rooms.end(),
                       &probe_room,
                       comp);
}

static Rooms_t::iterator find_room_iter(Schedule& schedule,
                                        const int room_number)
{
    auto room_iter = find_room_iter_helper(schedule, room_number);

    // If a lower bound was found but it is not the Room we are trying to find
    // we want to return end() to indicate the Room was not found
    if (room_iter != schedule.m_rooms.end() &&
        (*room_iter)->get_room_number() != room_number)
    {
        room_iter = schedule.m_rooms.end();
    }

    return room_iter;
}

//static Meeting& find_meeting(Schedule& schedule,
                             //const int room_number,
                             //const int meeting_time)
//{
    //Room& room = find_room(schedule, room_number);
    //return room.get_Meeting(meeting_time);
//}

static People_t::iterator find_person_iter(Schedule& schedule,
                                           const string& lastname)
{
    const Person probe(lastname);
    return schedule.m_people.find(&probe);
}

static const Person& find_person(Schedule& schedule, const string& lastname) {
    auto person_iter = find_person_iter(schedule, lastname);
    if (person_iter == schedule.m_people.end()) {
        throw Error("No person with that name!");
    }

    return **person_iter;
}

