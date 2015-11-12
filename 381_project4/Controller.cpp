#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Utility.h"
#include "Geometry.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include <iostream>
#include <exception>
#include <algorithm>
#include <utility>
#include <ctype.h>
#include <cassert>

using namespace std;

Controller::Controller() : mp_view(nullptr), mp_current_agent(nullptr)
{
}

static int read_int() {
    int return_val;
    cin >> return_val;

    if (!cin.good()) {
        throw Error("Expected an integer!");
    }

    return return_val;
}

static double read_double() {
    double return_val;
    cin >> return_val;

    if (!cin.good()) {
        throw Error("Expected a double!");
    }

    return return_val;
}

// returns true if string contains only alphanumeric characters
static bool string_is_alnum(const string& str) {
    auto iter = find_if(str.begin(),
                        str.end(),
                        [](char c)->bool{ return !isalnum(c); });
    return iter == str.end();
}

void Controller::init_commands() {
    m_view_commands["default"] = &Controller::view_default_command;
    m_view_commands["size"] = &Controller::view_size_command;
    m_view_commands["zoom"] = &Controller::view_zoom_command;
    m_view_commands["pan"] = &Controller::view_pan_command;

    m_agent_commands["move"] = &Controller::agent_move_command;
    m_agent_commands["work"] = &Controller::agent_work_command;
    m_agent_commands["attack"] = &Controller::agent_attack_command;
    m_agent_commands["stop"] = &Controller::agent_stop_command;

    m_program_commands["status"] = &Controller::status_command;
    m_program_commands["show"] = &Controller::show_command;
    m_program_commands["go"] = &Controller::go_command;
    m_program_commands["build"] = &Controller::build_command;
    m_program_commands["train"] = &Controller::train_command;
}

static void discard_rest_of_line(std::istream& is) {
    is.clear();
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

Controller::Controller_fp_t Controller::get_command_helper(Command_map_t& commands,
                                                           const string& command)
{
    auto iter = commands.find(command);

    if (iter == commands.end()) {
        return nullptr;
    }

    return iter->second;
}

Controller::Controller_fp_t Controller::get_agent_command() {
    string command;
    cin >> command;

    // Try to find Agent command function
    Controller_fp_t command_ptr = get_command_helper(m_agent_commands, command);

    // if none found, throw error
    if (!command_ptr) {
        throw Error("Unrecognized command!");
    }

    return command_ptr;
}

Controller::Controller_fp_t Controller::get_view_program_command(const string& command) {
    // try to find program command
    Controller_fp_t command_ptr = get_command_helper(m_program_commands, command);

    // if program command not found, try to find view command
    if (!command_ptr) {
        command_ptr = get_command_helper(m_view_commands, command);
    }

    // if none found, throw error
    if (!command_ptr) {
        throw Error("Unrecognized command!");
    }

    return command_ptr;
}

void Controller::run() {
    // setup up View
    View view;
    mp_view = &view;
    g_Model_ptr->attach(mp_view);

    // Fill command containers
    init_commands();

    string first_word;
    Controller_fp_t command_ptr = nullptr;

    // main program loop, exited when user enters "quit" command
    while (true) {
        try {
            cout << "\nTime " << g_Model_ptr->get_time() << ": Enter command: ";

            cin >> first_word;

            // Check if user wants to quit
            if (first_word.compare("quit") == 0) {
                break; // break out of main program loop
            }

            // Check if first word is name of an Agent and set the Agent ptr
            // then execute an Agent command
            if (g_Model_ptr->is_agent_present(first_word)) {
                mp_current_agent = g_Model_ptr->get_agent_ptr(first_word);
                if (!mp_current_agent->is_alive()) {
                    throw Error("Agent is not alive!");
                }

                // Find Agent command, throws Error if none found
                command_ptr = get_agent_command();
            }
            // Otherwise check if user input a different command and execute it
            else {
                // Try to find a view or program-wide command, Error thrown
                // if first_word is not a valid command
                command_ptr = get_view_program_command(first_word);
            }

            // Execute command
            (this->*(command_ptr))();
        }
        catch (exception& e) {
            cout << e.what() << endl;

            // If we've reached EOF allow program to exit normally
            if (cin.eof()) break;

            discard_rest_of_line(cin);
        }
        catch (...) {
            cout << "Unknown exception caught!" << endl;
        }
    }

    // Tear down View
    g_Model_ptr->detach(mp_view);
    mp_view = nullptr;

    cout << "Done" << endl;
}

// View commands from spec
void Controller::view_default_command() {
    assert(mp_view);
    mp_view->set_defaults();
}

void Controller::view_size_command() {
    assert(mp_view);
    int new_size = read_int();
    mp_view->set_size(new_size);
}

void Controller::view_zoom_command() {
    assert(mp_view);
    double new_scale = read_double();
    mp_view->set_scale(new_scale);
}

static Point read_point() {
    double new_x = read_double();
    double new_y = read_double();
    return Point(new_x, new_y);
}

void Controller::view_pan_command() {
    assert(mp_view);
    Point new_origin = read_point();
    mp_view->set_origin(new_origin);
}

void Controller::agent_move_command() {
    assert(mp_current_agent);
    Point move_pt = read_point();
    mp_current_agent->move_to(move_pt);
}

// Attempts to get a Structure from a name read from input
// throws an Error if no Structure with the name is found
static Structure* read_structure_from_input() {
    // Read and find Structure, throws Error if not found
    string structure_name;
    cin >> structure_name;
    return g_Model_ptr->get_structure_ptr(structure_name);
}

void Controller::agent_work_command() {
    assert(mp_current_agent);

    // Read and find these Structures, throws Error if either is not found
    Structure* source_ptr = read_structure_from_input();
    Structure* destination_ptr = read_structure_from_input();

    assert(source_ptr);
    assert(destination_ptr);

    // Tell Agent to work, throws Error if Agent cannot work
    mp_current_agent->start_working(source_ptr, destination_ptr);
}

void Controller::agent_attack_command() {
    assert(mp_current_agent);

    // read name for target to attack
    string target_name;
    cin >> target_name;

    // Find target Agent, throws Error if Agent not found
    Agent* target_ptr = g_Model_ptr->get_agent_ptr(target_name);
    assert(target_ptr);

    // Attack target if possible, throws Error if Agent cannot attack
    mp_current_agent->start_attacking(target_ptr);
}

void Controller::agent_stop_command() {
    assert(mp_current_agent);
    mp_current_agent->stop();
}

void Controller::status_command() {
    g_Model_ptr->describe();
}

void Controller::show_command() {
    assert(mp_view);
    mp_view->draw();
}

void Controller::go_command() {
    g_Model_ptr->update();
}

static void read_name(string& name) {
    cin >> name;

    // Check if input name is a valid object name
    if (name.length() < 2 || !string_is_alnum(name) ||
        g_Model_ptr->is_name_in_use(name))
    {
        throw Error("Invalid name for new object!");
    }
}

// Data for creating a new Sim_object
struct New_obj_info {
    string name;
    string type;
    Point start_pt;
};

// Read data for creating new Sim_object from input
static void read_new_obj_info(New_obj_info& info) {
    // read in and validate name
    read_name(info.name);

    // read in type of Structure to create, no validation yet
    cin >> info.type;

    // read in start point
    info.start_pt = read_point();
}

void Controller::build_command() {
    New_obj_info info;
    read_new_obj_info(info);

    // create new Structure if information read in is valid
    Structure* new_structure = create_structure(info.name,
                                                info.type,
                                                info.start_pt);

    // Add new Structure to Model
    g_Model_ptr->add_structure(new_structure);
}

void Controller::train_command() {
    New_obj_info info;
    read_new_obj_info(info);

    // create new Structure if information read in is valid
    Agent* new_agent = create_agent(info.name, info.type, info.start_pt);

    // Add new Structure to Model
    g_Model_ptr->add_agent(new_agent);
}
