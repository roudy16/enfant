#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "Utility.h"

#include <string>
#include <set>
#include <utility>
#include <iostream>
#include <fstream>
#include <new>
#include <cassert>

using namespace std;

template <typename T>
struct Comp_objects_by_ptr{
    bool operator()(const T* lhs, const T* rhs) { return *lhs < *rhs; }
};

// TODO Git Red uf dis, mak haxmap
// Holds the pair of characters used to determine input command
struct Command_pair {
    char command0;
    char command1;
};

// We will use the compiler created default/move ctors, we must define our
// own dtor to clean up allocated rooms and people.
class Schedule {
public:
    ~Schedule();

/* ########################### */
/* COMMAND FUNCTIONS FROM SPEC */
/* ########################### */

    void print_person_command() const;
    void print_room_command() const;
    void print_meeting_command() const;
    void print_all_meetings() const;
    void print_all_people() const;
    void print_memory_allocations() const;

    void add_to_people_list();
    void add_room();
    void add_meeting();
    void add_person_to_meeting_in_room();

    void reschedule_meeting();

    void delete_individual_command();
    void delete_room_command();
    void delete_meeting_command();
    void delete_participant_command();
    void delete_all_individual_command();
    void delete_schedule_command();
    void deallocate_all();

    void save_data_command();
    void load_data_command();

private:
    using Rooms_t = set<Room*, Comp_objects_by_ptr<Room>>;
    using People_t = set<Person*, Comp_objects_by_ptr<Person>>;

    // Find a room in the schedule by number if it exists, throw an Error
    // if no such Room is found
    Rooms_t::const_iterator find_room(const int room_number) const;
    Rooms_t::iterator find_room(const int room_number);

    // Find a person in the schedule by lastname if it exists, throw an Error
    // if no such Person is found
    People_t::const_iterator find_person(const string& lastname) const;
    People_t::iterator find_person(const string& lastname);

    void delete_schedule_helper();

    // Reads and checks validity of time read from stream, throws error if reading
    // fails or value is invalid
    int read_time_from_stream(std::istream& is) const;

    // Reads and checks validity of room number read from stream, throws error if
    // reading fails or value is invalid
    int read_room_number_from_stream(std::istream& is) const;

    Rooms_t m_rooms;
    People_t m_people;
};


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// Loop that contains main switch for processing command arguments from stdin
static void main_loop(Schedule& schedule);

// Functions containing switch statements for related commands
static void print_switch(const char command, const Schedule& schedule);
static void add_switch(const char command, Schedule& schedule);
static void delete_switch(const char command, Schedule& schedule);

// ignore the rest of a line
static void discard_rest_of_line(std::istream& is);

// Validate value ranges according to project specification, returns
// non-zero if argument is valid, returns zero otherwise.
static bool is_room_range_valid(const int room_number);
static bool is_time_range_valid(int time);

// Returns an int read from the stream, throws an error if reading fails
static int read_int_from_stream(std::istream& is);

// Reads in two characters from cin and returns them in a Command_pair
static Command_pair get_command_from_input();




/* #################### */
/* FUNCTION DEFINITIONS */
/* #################### */

int main() {
    Schedule schedule;

    main_loop(schedule);

    schedule.deallocate_all();

    printf("Done\n");
}

static Command_pair get_command_from_input()
{
    cout << "\nEnter command: ";

    Command_pair commands;

    while (isspace(cin.peek())) {
        cin.ignore();
    }

    cin.get(commands.command0);

    while (isspace(cin.peek())) {
        cin.ignore();
    }

    cin.get(commands.command1);

    return commands;
}

static void main_loop(Schedule& schedule)
{
    bool quit_flag = false;
    Command_pair commands;

    while (!quit_flag)
    {
        try {
            commands = get_command_from_input();

            switch (commands.command0)
            {
            case 'p':
                switch (commands.command1)
                {
                case 'i':
                    schedule.print_person_command();
                    break;
                case 'r':
                    schedule.print_room_command();
                    break;
                case 'm':
                    schedule.print_meeting_command();
                    break;
                case 's':
                    schedule.print_all_meetings();
                    break;
                case 'g':
                    schedule.print_all_people();
                    break;
                case 'a':
                    schedule.print_memory_allocations();
                    break;
                default:
                    throw Error("Unrecognized command!");
                }
                break;
            case 'a':
                switch (commands.command1)
                {
                case 'i':
                    schedule.add_to_people_list();
                    break;
                case 'r':
                    schedule.add_room();
                    break;
                case 'm':
                    schedule.add_meeting();
                    break;
                case 'p':
                    schedule.add_person_to_meeting_in_room();
                    break;
                default:
                    throw Error("Unrecognized command!");
                }
                break;
            case 'r':
                if (commands.command1 == 'm') {
                    schedule.reschedule_meeting();
                }
                else {
                    throw Error("Unrecognized command!");
                }
                break;
            case 'd':
                switch (commands.command1)
                {
                case 'i':
                    schedule.delete_individual_command();
                    break;
                case 'r':
                    schedule.delete_room_command();
                    break;
                case 'm':
                    schedule.delete_meeting_command();
                    break;
                case 'p':
                    schedule.delete_participant_command();
                    break;
                case 's':
                    schedule.delete_schedule_command();
                    break;
                case 'g':
                    schedule.delete_all_individual_command();
                    break;
                case 'a':
                    schedule.deallocate_all();
                    break;
                default:
                    throw Error("Unrecognized command!");
                }
                break;
            case 's':
                if (commands.command1 == 'd') {
                    schedule.save_data_command();
                }
                else {
                    throw Error("Unrecognized command!");
                }
                break;
            case 'l':
                if (commands.command1 == 'd') {
                    schedule.load_data_command();
                }
                else {
                    throw Error("Unrecognized command!");
                }
                break;
            case 'q':
                if (commands.command1 == 'q')
                {
                    quit_flag = 1;
                    continue;
                }
                /* no break */
            default:
                throw Error("Unrecognized command!");
            }
        } // End try-block
        catch (Error& e) {
            discard_rest_of_line(cin);
            cout << e.msg;
        }
        catch (std::bad_alloc& e){
            // TODO handle bad allocs, look at spec for what to do
        } 
        catch (...) {
            // catch all for stuff thrown from not my code
        }
    } // End while-loop
}

static void discard_rest_of_line(std::istream& is) {
    is.clear();
    while (is && is.get() != '\n') {
        ;
    }
}

static bool is_room_range_valid(const int room_number) {
    return room_number > 0;
}

static bool is_time_range_valid(int time) {
    time = convert_time_to_24_hour(time);
    return time >= k_EARLIEST_MEETING_TIME && time <= k_LATEST_MEETING_TIME;
}

void Schedule::print_person_command() const {
    string lastname;
    cin >> lastname;

    auto person_iter = find_person(lastname);

    cout << **person_iter << endl;
}

void Schedule::print_room_command() const {
    int room_number;
    cin >> room_number;


    auto iter = find_room(room_number);
    cout << **iter;
}


void Schedule::print_meeting_command() const {
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);
    int meeting_time = read_time_from_stream(cin);
    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);
    cout << meeting;
}

void Schedule::print_all_meetings() const {
    if (m_rooms.empty()) {
        cout << "List of rooms is empty" << endl;
    }
    else {
        cout << "Information for " << m_rooms.size() << " rooms:" << endl;
        for (auto room_p : m_rooms) {
            cout << *room_p;
        }
    }
}

void Schedule::print_all_people() const {
    if (m_people.empty()) {
        cout << "List of people is empty" << endl;
    }
    else {
        cout << "Information for " << m_people.size() << " people:" << endl;
        for (auto person_p : m_people) {
            cout << *person_p << endl;
        }
    }
}

void Schedule::print_memory_allocations() const {
    int total_meetings = 0;
    for (auto room_p : m_rooms) {
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


void Schedule::add_to_people_list(){
    string firstname, lastname, phoneno;

    cin >> firstname >> lastname >> phoneno;

    Person probe(lastname);
    auto iter = m_people.find(&probe);
    if (iter != m_people.end()) {
        throw Error("There is already a person with this last name!\n");
    }

    m_people.insert(new Person(firstname, lastname, phoneno));
    cout << "Person " << lastname << " added" << endl;
}

void Schedule::add_room(){
    int room_number = read_room_number_from_stream(cin);

    // Ensure that the room does not already exist
    Room probe_room(room_number);
    Rooms_t::const_iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter != m_rooms.end()) {
        throw Error("There is already a room with this number!");
    }

    m_rooms.insert(new Room(room_number));
    cout << "Room " << room_number << " added" << endl;
}

void Schedule::add_meeting(){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    string topic;
    cin >> topic;

    Meeting new_meeting(meeting_time, topic);

    (*room_iter)->add_Meeting(std::move(new_meeting));

    cout << "Meeting added at " << meeting_time << endl;
}

void Schedule::add_person_to_meeting_in_room(){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    auto person_iter = find_person(lastname);

    meeting.add_participant(*person_iter);
    cout << "Participant " << lastname << " added" << endl;
}

void Schedule::reschedule_meeting() {
    /* we want to find the old room and meeting then find the new room.
       if any errors occur during this process the called functions will
       throw an appropriate exception 
    */
    int old_room_number = read_room_number_from_stream(cin);
    auto old_room_iter = find_room(old_room_number);
    int old_meeting_time = read_time_from_stream(cin);
    Meeting& old_meeting = (*old_room_iter)->get_Meeting(old_meeting_time);

    int new_room_number = read_room_number_from_stream(cin);
    auto new_room_iter = find_room(new_room_number);
    int new_meeting_time = read_time_from_stream(cin);
    Meeting new_meeting(old_meeting);

    new_meeting.set_time(new_meeting_time);

    (*old_room_iter)->remove_Meeting(old_meeting_time);
    (*new_room_iter)->add_Meeting(std::move(new_meeting));

    cout << "Meeting rescheduled to room " << new_room_number
         << " at " << new_meeting_time << endl;
}

void Schedule::delete_individual_command(){
    string lastname;
    cin >> lastname;

    // Make sure the person exists
    auto iter = find_person(lastname);

    // If the person is scheduled for a meeting we cannot delete them
    for (auto room_ptr : m_rooms) {
        if (room_ptr->is_participant_present(*iter)) {
            throw Error("This person is a participant in a meeting!");
        }
    }

    // Free memory allocated for Person object before erasing node
    delete *iter;
    m_people.erase(iter);
    cout << "Person " << lastname << " deleted" << endl;
}

void Schedule::delete_room_command(){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);

    // Free memory allocated for Room object before erasing node
    delete *room_iter;
    m_rooms.erase(room_iter);
    cout << "Room " << room_number << " deleted" << endl;
}

void Schedule::delete_meeting_command(){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    (*room_iter)->remove_Meeting(meeting_time);
    cout << "Meeting at " << meeting_time << " deleted" << endl;
}

void Schedule::delete_participant_command(){
    int room_number = read_room_number_from_stream(cin);
    auto room_iter = find_room(room_number);
    int meeting_time = read_time_from_stream(cin);

    Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);

    string lastname;
    cin >> lastname;

    auto person_iter = find_person(lastname);

    meeting.remove_participant(*person_iter);
    cout << "Participant " << lastname << " deleted" << endl;
}

bool room_has_meetings(const Room *const p) {
    return p->has_Meetings();
}

void Schedule::delete_all_individual_command(){
    // Check to see if any meetings exist in any room
    if (apply_if(m_rooms.begin(), m_rooms.end(), room_has_meetings))
    {
        throw Error("Cannot clear people list unless there are no meetings!");
    }

    // deallocate all Person objects in the people list
    for (auto p : m_people) {
        delete p;
    }

    m_people.clear();
    cout << "All persons deleted" << endl;
}

void Schedule::delete_schedule_helper() {
    for (auto room_ptr : m_rooms) {
        room_ptr->clear_Meetings();
    }
}

void Schedule::delete_schedule_command(){
    delete_schedule_helper();
    cout << "All meetings deleted" << endl;
}

void Schedule::deallocate_all(){
    delete_schedule_helper();

    for (auto room_ptr : m_rooms) {
        delete room_ptr;
    }
    m_rooms.clear();

    cout << "All rooms and meetings deleted" << endl;

    delete_all_individual_command();
}

void Schedule::save_data_command(){
    string filename;
    cin >> filename;

    std::ofstream ofs(filename.c_str());
    if (!ofs.good()) {
        throw Error("Could not open file!");
    }

    ofs << m_people.size() << endl;
    for (auto person_ptr : m_people) {
        person_ptr->save(ofs);
    }

    ofs << m_rooms.size() << endl;
    for (auto room_ptr : m_rooms) {
        room_ptr->save(ofs);
    }

    cout << "Data saved" << endl;
}

void Schedule::load_data_command(){
    string filename;
    cin >> filename;

    std::ifstream ifs(filename.c_str());
    if (!ifs.good()) {
        throw Error("Could not open file!");
    }

    People_t old_people = std::move(m_people);
    Rooms_t old_rooms = std::move(m_rooms);

    try {
        int number_of_people;
        ifs >> number_of_people;
        if (!ifs.good()) {
            throw Error("Invalid data found in file!");
        }

        for (int i = 0; i < number_of_people; ++i) {
            m_people.insert(new Person(ifs));
        }

        int number_of_rooms;
        ifs >> number_of_rooms;
        if (!ifs.good()) {
            throw Error("Invalid data found in file!");
        }

        for (int i = 0; i < number_of_rooms; ++i) {
            m_rooms.insert(new Room(ifs, m_people));
        }

        cout << "Data loaded" << endl;
    }
    catch (...) {
        // Catch any exception then clean up the work in progress by moving
        // the rooms and people lists to their original state and deleting the
        // memory allocated before exception
        for (auto room_ptr : m_rooms) {
            delete room_ptr;
        }
        for (auto person_ptr : m_people) {
            delete person_ptr;
        }

        m_people.swap(old_people);
        m_rooms.swap(old_rooms);

        throw;
    }

    // deallocate old data before destrucor
    for (auto room_ptr : old_rooms) {
        delete room_ptr;
    }
    for (auto person_ptr : old_people) {
        delete person_ptr;
    }
}

auto Schedule::find_room(const int room_number) ->Rooms_t::iterator {
    Room probe_room(room_number);
    Rooms_t::iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        throw Error("No room with that number!");
    }

    return room_iter;
}

auto Schedule::find_room(const int room_number) const ->Rooms_t::const_iterator {
    Room probe_room(room_number);
    Rooms_t::const_iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        throw Error("No room with that number!");
    }

    return room_iter;
}

auto Schedule::find_person(const string& lastname) const ->People_t::const_iterator {
    Person probe(lastname);
    auto person_iter = m_people.find(&probe);
    if (person_iter == m_people.end()) {
        throw Error("No person with that name!");
    }

    return person_iter;
}

auto Schedule::find_person(const string& lastname) ->People_t::iterator {
    Person probe(lastname);
    auto person_iter = m_people.find(&probe);
    if (person_iter == m_people.end()) {
        throw Error("No person with that name!");
    }

    return person_iter;
}

static int read_int_from_stream(std::istream& is) {
    int val;
    is >> val;

    if (!is.good()) {
        throw Error("Could not read an integer value!");
    }

    return val;
}

int Schedule::read_time_from_stream(std::istream& is) const {
    int time = read_int_from_stream(is);

    if (!is_time_range_valid(time)) {
        throw Error("Time is not in range!");
    }

    return time;
}

int Schedule::read_room_number_from_stream(std::istream& is) const {
    int room_number = read_int_from_stream(is);

    if (!is_room_range_valid(room_number)) {
        throw Error("Room number is not in range!");
    }

    return room_number;
}

Schedule::~Schedule() {
    for (auto p : m_rooms) {
        delete p;
    }

    for (auto p : m_people) {
        delete p;
    }
}
