#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <string>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <cassert>

using namespace std;

// We will use the compiler created default/move ctors, we must define our
// own dtor to clean up allocated rooms and people.
struct Schedule {
public:
    using Rooms_t = set<Room*, Comp_objects_by_ptr<Room>>;
    using People_t = set<const Person*, Comp_objects_by_ptr<const Person>>;

    Schedule();
    Schedule(Schedule&& original);
    Schedule& operator=(Schedule&& original);
    ~Schedule();

    void print_person();
    void print_room();
    void print_meeting();
    void print_all_meetings();
    void print_all_people();
    void print_memory_allocations();

    void add_to_people_list();
    void add_room();
    void add_meeting();
    void add_person_to_meeting_in_room();

    void reschedule_meeting();

    void delete_individual();
    void delete_room();
    void delete_meeting();
    void delete_participant();
    void delete_all_individual();
    void delete_schedule();
    void deallocate_all();

    void save_data();
    void load_data();

    int number_of_meetings() const;
    int number_of_rooms() const;

    void clear_all_meetings();
    void clear_all_rooms();
    void clear_all_people();

    // Find a room in the schedule by number if it exists, throw an Error
    // if no such Room is found
    const Room& find_room(const int room_number) const;
    Room& find_room(const int room_number);

    // Find a meeting in the schedule by number if it exists, throw an Error
    // if no such Meeting is found
    const Meeting& find_meeting(const int room_number, const int meeting_time) const;
    Meeting& find_meeting(const int room_number, const int meeting_time);

    // Find a person in the schedule by lastname if it exists, throw an Error
    // if no such Person is found
    const Person& find_person(const string& lastname) const;

    Rooms_t m_rooms;
    People_t m_people;
};


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// ignore the rest of a line
static void discard_rest_of_line(std::istream& is);

// Validate value ranges according to project specification, returns
// non-zero if argument is valid, returns zero otherwise.
static bool is_room_range_valid(const int room_number);
static bool is_time_range_valid(int time);

// Returns an int read from the stream, throws an error if reading fails
static int read_int_from_stream(std::istream& is);

// Reads and checks validity of time read from stream, throws error if reading
// fails or value is invalid
int read_time_from_stream(std::istream& is);

// Reads and checks validity of room number read from stream, throws error if
// reading fails or value is invalid
int read_room_number_from_stream(std::istream& is);


static void print_person_command(Schedule& schedule);
static void print_room_command(Schedule& schedule);
static void print_meeting_command(Schedule& schedule);
static void print_all_meetings_command(Schedule& schedule);
static void print_all_people_command(Schedule& schedule);
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
    commands["pa"] = &print_memory_allocations_command;

    commands["ai"] = &add_to_people_list_command;
    commands["ar"] = &add_room_command;
    commands["am"] = &add_meeting_command;
    commands["ap"] = &add_person_to_meeting_in_room_command;

    commands["di"] = &deallocate_all_command;
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

static void get_command_from_input(string &command_input)
{
    cout << "\nEnter command: ";

    while (isspace(cin.peek())) {
        cin.ignore();
    }

    command_input += cin.get();

    while (isspace(cin.peek())) {
        cin.ignore();
    }

    command_input += cin.get();
}

static void discard_rest_of_line(std::istream& is) {
    is.clear();
    while (is && is.get() != '\n') {
        ;
    }
}

int main() {
    map<string, void (*)(Schedule&)> commands;
    init_command_table(commands);

    Schedule schedule;
    string command_input;

    while (true) {
        try {
            command_input.clear();
            get_command_from_input(command_input);

            auto command_iter = commands.find(command_input);
            if (command_iter == commands.end()) {
                // Check if 'quit' command was entered
                if (command_input.compare(0, 2, "qq")) {
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
            cout << e.msg;
        }
        catch (std::bad_alloc& e){
            // TODO handle bad allocs, look at spec for what to do
            cout << e.what() << endl;
        } 
        catch (...) {
            // catch all for stuff thrown from not my code
        }
    } // End while loop

    printf("Done\n");
    return 0;
}

static bool is_room_range_valid(const int room_number) {
    return room_number > 0;
}

static bool is_time_range_valid(int time) {
    time = convert_time_to_24_hour(time);
    return time >= k_EARLIEST_MEETING_TIME && time <= k_LATEST_MEETING_TIME;
}

void print_person_command(Schedule& schedule) {
    string lastname;
    cin >> lastname;

    auto person_iter = schedule.find_person(lastname);
    cout << **person_iter << endl;
}

void print_room_command(Schedule& schedule) {
    int room_number;
    cin >> room_number;

    auto iter = schedule.find_room(room_number);
    cout << **iter;
}

void print_meeting_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = schedule.find_room(room_number);
    int meeting_time = read_time_from_stream(cin);
    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);
    cout << meeting;
}

void print_all_meetings_command(Schedule& schedule) {
    if (schedule.m_rooms.empty()) {
        cout << "List of rooms is empty" << endl;
    }
    else {
        cout << "Information for " << schedule.m_rooms.size() << " rooms:" << endl;
        for (auto room_p : schedule.m_rooms) {
            cout << *room_p;
        }
    }
}

void print_all_people_command(Schedule& schedule) {
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

void print_memory_allocations_command(Schedule& schedule) {
    int total_meetings = 0;
    for (auto room_p : schedule.m_rooms) {
        total_meetings += room_p->get_number_Meetings();
    }

    //cout << "Memory allocations:" << '\n';
    //cout << "Strings: " << String::get_number() << " with "
         //<< String::get_total_allocation() << " bytes total" << '\n';
    //cout << "Persons: " << m_people.size() << '\n';
    //cout << "Meetings: " << total_meetings << '\n';
    //cout << "Rooms: " << m_rooms.size() << '\n';
    //cout << "Lists: " << g_Ordered_list_count << '\n';
    //cout << "List Nodes: " << g_Ordered_list_Node_count << endl;
}


void add_to_people_list_command(Schedule& schedule){
    string firstname, lastname, phoneno;
    cin >> firstname >> lastname >> phoneno;

    // Ensure the person does not already exist
    Person probe(lastname);
    auto iter = find(schedule.m_people.begin(), schedule.m_people.end(), &probe);
    if (iter != schedule.m_people.end()) {
        throw Error("There is already a person with this last name!\n");
    }

    // Add new Person to people list
    schedule.m_people.insert(new Person(firstname, lastname, phoneno));
    cout << "Person " << lastname << " added" << endl;
}

void add_room_command(Schedule& schedule) {
    int room_number = read_room_number_from_stream(cin);

    // Ensure that the room does not already exist
    Room probe_room(room_number);
    auto room_iter = find(schedule.m_rooms.begin(), schedule.m_rooms.end(), &probe_room);
    if (room_iter != schedule.m_rooms.end()) {
        throw Error("There is already a room with this number!");
    }

    // Add new Room to rooms list
    schedule.m_rooms.insert(new Room(room_number));
    cout << "Room " << room_number << " added" << endl;
}

void add_meeting_command(Schedule& schedule) {
    // Read and check room number for validity
    int room_number = read_room_number_from_stream(cin);

    Room& room = schedule.find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    string topic;
    cin >> topic;

    Meeting new_meeting(meeting_time, topic);

    (*room_iter)->add_Meeting(std::move(new_meeting));

    cout << "Meeting added at " << meeting_time << endl;
}

void add_person_to_meeting_in_room_command(Schedule& schedule){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = schedule.find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    auto person_iter = schedule.find_person(lastname);

    meeting.add_participant(*person_iter);
    cout << "Participant " << lastname << " added" << endl;
}

void reschedule_meeting_command(Schedule& schedule) {
    /* we want to find the old room and meeting then find the new room.
       if any errors occur during this process the called functions will
       throw an appropriate exception 
    */
    int old_room_number = read_room_number_from_stream(cin);
    auto old_room_iter = schedule.find_room(old_room_number);
    int old_meeting_time = read_time_from_stream(cin);
    Meeting& old_meeting = (*old_room_iter)->get_Meeting(old_meeting_time);

    int new_room_number = read_room_number_from_stream(cin);
    auto new_room_iter = schedule.find_room(new_room_number);
    int new_meeting_time = read_time_from_stream(cin);
    Meeting new_meeting(old_meeting);

    new_meeting.set_time(new_meeting_time);

    (*old_room_iter)->remove_Meeting(old_meeting_time);
    (*new_room_iter)->add_Meeting(std::move(new_meeting));

    cout << "Meeting rescheduled to room " << new_room_number
         << " at " << new_meeting_time << endl;
}

void delete_individual_command(Schedule& schedule){
    string lastname;
    cin >> lastname;

    // Make sure the person exists
    auto iter = schedule.find_person(lastname);

    // If the person is scheduled for a meeting we cannot delete them
    for (auto room_ptr : schedule.m_rooms) {
        if (room_ptr->is_participant_present(*iter)) {
            throw Error("This person is a participant in a meeting!");
        }
    }

    // Free memory allocated for Person object before erasing node
    delete *iter;
    schedule.m_people.erase(iter);
    cout << "Person " << lastname << " deleted" << endl;
}

void delete_room_command(Schedule& schedule){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = schedule.find_room(room_number);

    // Free memory allocated for Room object before erasing node
    delete *room_iter;
    schedule.m_rooms.erase(room_iter);
    cout << "Room " << room_number << " deleted" << endl;
}

void delete_meeting_command(Schedule& schedule){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = schedule.find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    (*room_iter)->remove_Meeting(meeting_time);
    cout << "Meeting at " << meeting_time << " deleted" << endl;
}

void delete_participant_command(Schedule& schedule){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = schedule.find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    auto person_iter = schedule.find_person(lastname);

    meeting.remove_participant(*person_iter);
    cout << "Participant " << lastname << " deleted" << endl;
}

int Schedule::number_of_meetings() const {
    int num_meetings = 0;
    // Check to see if any meetings exist in any room
    for_each(m_rooms.begin(), m_rooms.end(), [&num_meetings](const Room* room) 
                                             {
                                                 num_meetings += room->get_number_Meetings();
                                             } );
    return num_meetings;
}

int Schedule::number_of_rooms() const {
    return m_rooms.size();
}

void delete_all_individual_command(Schedule& schedule){
    if (schedule.number_of_meetings() > 0)
    {
        throw Error("Cannot clear people list unless there are no meetings!");
    }

    // deallocate all Person objects in the people list
    schedule.clear_all_people();
    cout << "All persons deleted" << endl;
}

void deallocate_all_command(Schedule& schedule) {
    schedule.deallocate_all();

    cout << "All rooms and meetings deleted\n"
         << "All persons deleted" << endl;
}

void delete_schedule_command(Schedule& schedule){
    schedule.clear_all_meetings();
    cout << "All meetings deleted" << endl;
}

void Schedule::clear_all_meetings() {
    for_each(m_rooms.begin(), m_rooms.end(), [](Room* rm){ rm->clear_Meetings(); });
}

void Schedule::clear_all_rooms() {
    // the one range for
    for (auto room_ptr : m_rooms) {
        delete room_ptr;
    }
    m_rooms.clear();
}

void Schedule::clear_all_people() {
    for_each(m_people.begin(), m_people.end(), [](const Person* p){ delete p; });
    m_people.clear();
}

void Schedule::deallocate_all(){
    clear_all_meetings();
    clear_all_rooms();
    clear_all_people();
}

void save_data_command(Schedule& schedule){
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

void load_data_command(Schedule& schedule){
    string filename;
    cin >> filename;

    std::ifstream ifs(filename.c_str());
    if (!ifs.good()) {
        throw Error("Could not open file!");
    }

    Schedule old_schedule(move(schedule));

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
            schedule.m_rooms.insert(new Room(ifs, schedule.m_people));
        }

        cout << "Data loaded" << endl;
    }
    catch (...) {
        // Catch any exception then clean up the work in progress by moving
        // the rooms and people lists to their original state and deleting the
        // memory allocated before exception

        // move assignment deallocates contents of lhs schedule
        schedule = move(old_schedule);

        throw;
    }
}

const Room& Schedule::find_room(const int room_number) const {
    Room probe_room(room_number);
    Rooms_t::const_iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        throw Error("No room with that number!");
    }

    return **room_iter;
}

Room& Schedule::find_room(const int room_number) {
    Room probe_room(room_number);
    Rooms_t::iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        throw Error("No room with that number!");
    }

    return **room_iter;
}

const Meeting& Schedule::find_meeting(const int room_number, const int meeting_time) const {
    const Room& room = find_room(room_number);
    return room.get_Meeting(meeting_time);
}

Meeting& Schedule::find_meeting(const int room_number, const int meeting_time) {
    Room room = find_room(room_number);
    return room.get_Meeting(meeting_time);
}

const Person& Schedule::find_person(const string& lastname) const {
    Person probe(lastname);
    auto person_iter = m_people.find(&probe);
    if (person_iter == m_people.end()) {
        throw Error("No person with that name!");
    }

    return **person_iter;
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

// Move contents of original into the new Schedule and leave the original
// Schedule in an empty state.
Schedule::Schedule(Schedule&& original)
    : m_rooms(move(original.m_rooms)), m_people(move(original.m_people))
{
    original.m_rooms.clear();
    original.m_people.clear();
}

// Ensure we deallocate memory we were using, move the contents of
// the original to LHS schedule and leave the original in empty state
Schedule& Schedule::operator=(Schedule&& original) 
{
    if (&original == this) {
        return *this;
    }

    clear_all_rooms();
    clear_all_people();
    m_rooms = move(original.m_rooms);
    m_people = move(original.m_people);
    original.m_rooms.clear();
    original.m_people.clear();

    return *this;
}

Schedule::~Schedule() {
    deallocate_all();
}
