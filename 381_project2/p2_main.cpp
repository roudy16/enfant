#include "Ordered_list.h"
#include "Meeting.h"
#include "Person.h"
#include "Room.h"
#include "String.h"
#include "Utility.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <new>
#include <cassert>

using Rooms_t = Ordered_list < Room* const, Less_than_ptr<Room* const> > ;
using People_t = Ordered_list < const Person*, Less_than_ptr<const Person*> > ;
using std::cout;
using std::cin;
using std::endl;

// We will use the compiler created default/move ctors, we must define our
// own dtor to clean up allocated rooms and people.
struct Schedule {
    Rooms_t m_rooms;
    People_t m_people;

    ~Schedule();

    // Find a room in the schedule by number if it exists, throw an Error
    // if no such Room is found
    Rooms_t::const_Iterator find_room(const int room_number) const;
    Rooms_t::Iterator find_room(const int room_number);
};

struct Command_pair {
    char command0;
    char command1;
};

// We can throw this and handle this specific exception accordingly
struct Unrecognized_command_exception {};


/* ############################ */
/* HELPER FUNCTION DECLARATIONS */
/* ############################ */


// Loop that contains main switch for processing command arguments from stdin
static void main_loop(Schedule& schedule);

// Functions containing switch statements for related commands
static void print_switch(const char command, const Schedule& schedule);
static void add_switch(const char command, Schedule& schedule);
static void delete_switch(const char command, Schedule& schedule);

// Reads in two characters from cin and returns them in a Command_pair
static Command_pair get_command_from_input();

// Returns a data ptr to first element in the container that compares equal
// to the passed in argument using the passed in compare function, returns
// NULL if none is found.
// TODO - this is probably not needed
template<typename T, typename OF>
static void* find_object_arg(const Ordered_list<T, OF> container, const void* arg_ptr);

// Validate value ranges according to project specification, returns
// non-zero if argument is valid, returns zero otherwise.
static bool check_room_range_valid(const int room_number);
static bool check_time_range_valid(int time);


/* ########################### */
/* COMMAND FUNCTIONS FROM SPEC */
/* ########################### */


static void print_person_command(const Schedule& schedule);
static void print_room_command(const Schedule& schedule);
static void print_meeting_command(const Schedule& schedule);
static void print_all_meetings(const Schedule& schedule);
static void print_all_people(const Schedule& schedule);
static void print_memory_allocations(const Schedule& schedule);

static void add_to_people_list(Schedule& schedule);
static void add_room(Schedule& schedule);
static void add_meeting(Schedule& schedule);
static void add_person_to_meeting_in_room(Schedule& schedule);

static void reschedule_meeting(const char command, Schedule& schedule);

static void delete_individual_command(Schedule& schedule);
static void delete_room_command(Schedule& schedule);
static void delete_meeting_command(Schedule& schedule);
static void delete_participant_command(Schedule& schedule);
static void delete_all_individual_command(Schedule& schedule);
static void delete_schedule_command(Schedule& schedule, bool quiet);
static void deallocate_all(Schedule& schedule, bool quiet);

static void save_data_command(const char command, Schedule& schedule);
static void load_data_command(const char command, Schedule& schedule);


/* #################### */
/* FUNCTION DEFINITIONS */
/* #################### */

int main() {
    Schedule schedule;

    main_loop(schedule);

    printf("Done\n");

    // TODO assert memory stuff 

    return 0;
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
                print_switch(commands.command1, schedule);
                break;
            case 'a':
                add_switch(commands.command1, schedule);
                break;
            case 'r':
                reschedule_meeting(commands.command1, schedule);
                break;
            case 'd':
                delete_switch(commands.command1, schedule);
                break;
            case 's':
                save_data_command(commands.command1, schedule);
                break;
            case 'l':
                load_data_command(commands.command1, schedule);
                break;
            case 'q':
                if (commands.command1 == 'q')
                {
                    quit_flag = 1;
                    continue;
                }
                /* no break */
            default:
                throw Unrecognized_command_exception();
            }
        }
        catch (String_exception& se) {
            cout << se.msg;
        }
        catch (Error& e) {
            cout << e.msg;
        }
        catch (Unrecognized_command_exception) {
            cout << "Unrecognized command!";
            discard_rest_of_line(cin);
        }
        catch (std::bad_alloc){

        } 
        catch (...) {

        }
    }
}

static void print_switch(const char command, const Schedule& schedule)
{
    switch (command)
    {
    case 'i':
        print_person_command(schedule);
        break;
    case 'r':
        print_room_command(schedule);
        break;
    case 'm':
        print_meeting_command(schedule);
        break;
    case 's':
        print_all_meetings(schedule);
        break;
    case 'g':
        print_all_people(schedule);
        break;
    case 'a':
        print_memory_allocations(schedule);
        break;
    default:
        throw Unrecognized_command_exception();
    }
}

static void add_switch(const char command, Schedule& schedule)
{
    switch (command)
    {
    case 'i':
        add_to_people_list(schedule);
        break;
    case 'r':
        add_room(schedule);
        break;
    case 'm':
        add_meeting(schedule);
        break;
    case 'p':
        add_person_to_meeting_in_room(schedule);
        break;
    default:
        throw Unrecognized_command_exception();
    }
}

static void delete_switch(const char command, Schedule& schedule)
{
    switch (command)
    {
    case 'i':
        delete_individual_command(schedule);
        break;
    case 'r':
        delete_room_command(schedule);
        break;
    case 'm':
        delete_meeting_command(schedule);
        break;
    case 'p':
        delete_participant_command(schedule);
        break;
    case 's':
        delete_schedule_command(schedule, false);
        break;
    case 'g':
        delete_all_individual_command(schedule);
        break;
    case 'a':
        deallocate_all(schedule, false);
        break;
    default:
        throw Unrecognized_command_exception();
    }
}

static bool check_room_range_valid(const int room_number) {
    return room_number > 0;
}

static bool check_time_range_valid(int time) {
    time = convert_time_to_24_hour(time);
    return time >= k_EARLIEST_MEETING_TIME && time <= k_LATEST_MEETING_TIME;
}

static void load_data_error(Schedule& bad_schedule, std::ifstream& ifs){

}

static void print_person_command(const Schedule& schedule){
    String lastname;
    cin >> lastname;

    Person probe(lastname);
    auto iter = schedule.m_people.find(&probe);

    if (iter == schedule.m_people.end()) {
        // TODO throw error
        discard_rest_of_line(cin);
        throw Error("No person with that name!");
    }
    else {
        cout << **iter << endl;
    }
}

static void print_room_command(const Schedule& schedule){
    int room_number;
    cin >> room_number;

    if (!check_room_range_valid(room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto iter = schedule.find_room(room_number);
    cout << **iter;
}


static void print_meeting_command(const Schedule& schedule){
    int room_number;
    int meeting_time;

    cin >> room_number;
    if (!check_room_range_valid(room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto room_iter = schedule.find_room(room_number);

    cin >> meeting_time;
    if (!check_time_range_valid(meeting_time)) {
        discard_rest_of_line(cin);
        throw Error("Time is not in range!");
    }

    try {
        Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);
        cout << meeting;
    }
    catch (Error& e){
        discard_rest_of_line(cin);
        throw;
    }
}

static void print_all_meetings(const Schedule& schedule){
    if (schedule.m_rooms.empty()) {
        cout << "List of rooms is empty" << endl;
    }
    else {
        for (auto room_p : schedule.m_rooms) {
            cout << *room_p;
        }
    }
}

static void print_all_people(const Schedule& schedule){
    if (schedule.m_people.empty()) {
        cout << "List of people is empty" << endl;
    }
    else {
        for (auto person_p : schedule.m_people) {
            cout << *person_p << endl;
        }
    }
}

static void print_memory_allocations(const Schedule& schedule){
    int total_meetings = 0;
    for (auto room_p : schedule.m_rooms) {
        total_meetings += room_p->get_number_Meetings();
    }

    cout << "Memory allocations:" << '\n';
    cout << "Strings: " << String::get_number() << " with "
         << String::get_total_allocation() << " bytes total" << '\n';
    cout << "Persons: " << schedule.m_people.size() << '\n';
    cout << "Meetings: " << total_meetings << '\n';
    cout << "Rooms: " << schedule.m_rooms.size() << '\n';
    cout << "Lists: " << g_Ordered_list_count << '\n';
    cout << "List Nodes: " << g_Ordered_list_Node_count << endl;
}


static void add_to_people_list(Schedule& schedule){
    String firstname, lastname, phoneno;

    cin >> firstname >> lastname >> phoneno;

    Person probe(lastname);
    auto iter = schedule.m_people.find(&probe);
    if (iter != schedule.m_people.end()) {
        discard_rest_of_line(cin);
        throw Error("There is already a person with this last name!\n");
    }

    schedule.m_people.insert(new Person(firstname, lastname, phoneno));
    cout << "Person " << lastname << " added" << endl;
}

static void add_room(Schedule& schedule){
    int room_number;
    cin >> room_number;

    if (!check_room_range_valid(room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    Room probe_room(room_number);
    Rooms_t::const_Iterator room_iter = schedule.m_rooms.find(&probe_room);
    if (room_iter != schedule.m_rooms.end()) {
        discard_rest_of_line(cin);
        throw Error("There is already a room with this number!");
    }

    schedule.m_rooms.insert(new Room(room_number));
    cout << "Room " << room_number << " added" << endl;
}

static void add_meeting(Schedule& schedule){
    int room_number;
    int meeting_time;

    cin >> room_number;
    if (!check_room_range_valid(room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto room_iter = schedule.find_room(room_number);

    cin >> meeting_time;
    if (!check_time_range_valid(meeting_time)) {
        discard_rest_of_line(cin);
        throw Error("Time is not in range!");
    }

    String topic;
    cin >> topic;

    Meeting new_meeting(meeting_time, topic);

    try {
        (*room_iter)->add_Meeting(std::move(new_meeting));
    }
    catch (Error& e) {
        discard_rest_of_line(cin);
        throw;
    }

    cout << "Meeting added at " << meeting_time << endl;
}

static void add_person_to_meeting_in_room(Schedule& schedule){
    int room_number;
    int meeting_time;

    cin >> room_number;
    if (!check_room_range_valid(room_number)) {

        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto room_iter = schedule.find_room(room_number);

    cin >> meeting_time;
    if (!check_time_range_valid(meeting_time)) {
        discard_rest_of_line(cin);
        throw Error("Time is not in range!");
    }

    try {
        Meeting& meeting = (*room_iter)->get_Meeting(meeting_time);
        String lastname;
        cin >> lastname;

        Person probe(lastname);
        auto person_iter = schedule.m_people.find(&probe);

        if (person_iter == schedule.m_people.end()) {
            throw Error("No person with that name!");
        }

        meeting.add_participant(*person_iter);
        cout << "Participant " << lastname << " added" << endl;
    }
    catch (Error &e) {
        discard_rest_of_line(cin);
        throw;
    }
}

static void reschedule_meeting(const char command, Schedule& schedule) {
    if (command != 'm'){
        throw Unrecognized_command_exception();
    }

    int old_room_number;
    int old_meeting_time;
    int new_room_number;
    int new_meeting_time;

    cin >> old_room_number;
    if (!check_room_range_valid(old_room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto old_room_iter = schedule.find_room(old_room_number);

    cin >> old_meeting_time;
    if (!check_time_range_valid(old_meeting_time)) {
        discard_rest_of_line(cin);
        throw Error("Time is not in range!");
    }

    Meeting& old_meeting = (*old_room_iter)->get_Meeting(old_meeting_time);

    cin >> new_room_number;
    if (!check_room_range_valid(new_room_number)) {
        discard_rest_of_line(cin);
        throw Error("Room number is not in range!");
    }

    auto new_room_iter = schedule.find_room(new_room_number);

    cin >> new_meeting_time;
    if (!check_time_range_valid(new_meeting_time)) {
        discard_rest_of_line(cin);
        throw Error("Time is not in range!");
    }

    Meeting new_meeting(std::move(old_meeting));
    new_meeting.set_time(new_meeting_time);

    (*old_room_iter)->remove_Meeting(old_meeting_time);
    (*new_room_iter)->add_Meeting(std::move(new_meeting));

    cout << "Meeting rescheduled to room " << new_room_number
         << " at " << new_meeting_time << endl;
}

static void delete_individual_command(Schedule& schedule){

}

static void delete_room_command(Schedule& schedule){

}

static void delete_meeting_command(Schedule& schedule){

}

static void delete_participant_command(Schedule& schedule){

}

static void delete_all_individual_command(Schedule& schedule){

}

static void delete_schedule_command(Schedule& schedule, bool quiet){

}

static void deallocate_all(Schedule& schedule, bool quiet){

}


static void save_data_command(const char command, Schedule& schedule){

}

static void load_data_command(const char command, Schedule& schedule){

}

Rooms_t::Iterator Schedule::find_room(const int room_number) {
    Room probe_room(room_number);
    Rooms_t::Iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        discard_rest_of_line(cin);
        throw Error("No room with that number!");
    }

    return room_iter;
}

Rooms_t::const_Iterator Schedule::find_room(const int room_number) const {
    Room probe_room(room_number);
    Rooms_t::const_Iterator room_iter = m_rooms.find(&probe_room);
    if (room_iter == m_rooms.end()) {
        discard_rest_of_line(cin);
        throw Error("No room with that number!");
    }

    return room_iter;
}

Schedule::~Schedule() {
    for (auto p : m_rooms) {
        delete p;
    }

    for (auto p : m_people) {
        delete p;
    }
}
