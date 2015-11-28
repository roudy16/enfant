#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Utility.h"
#include "Geometry.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Agent.h"
#include "Structure.h"
#include <iostream>
#include <exception>
#include <algorithm>
#include <functional>
#include <utility>
#include <ctype.h>
#include <cassert>

using namespace std;
using namespace std::placeholders;

namespace {
    // Data for creating a new Sim_object
    struct New_obj_info {
        string name;
        string type;
        Point start_pt;
    };
}

// Read two doubles in from input then return Point created from those
// throws Error if doubles cannot be read
static Point read_point();

// Read and find Structure, throws Error if not found
static shared_ptr<Structure> read_structure_from_input();

// Read a new object name from input, throws Error if name is too short,
// is not alphanumeric or another object already is using that name
static void read_name(string& name);

// Read data for creating new Sim_object from input
static void read_new_obj_info(New_obj_info& info);

Controller::Controller()
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

Controller::Agent_command_map_t::mapped_type Controller::get_command_helper(Agent_command_map_t& commands,
    const std::string& command)
{
    auto iter = commands.find(command);

    if (iter == commands.end()) {
        return nullptr;
    }

    return iter->second;
}

Controller::Command_map_t::mapped_type Controller::get_command_helper(Command_map_t& commands,
    const std::string& command)
{
    auto iter = commands.find(command);

    if (iter == commands.end()) {
        return nullptr;
    }

    return iter->second;
}

Controller::Controller_agent_fp_t Controller::get_agent_command() {
    string command;
    cin >> command;

    // Try to find Agent command function
    Controller_agent_fp_t command_ptr = get_command_helper(m_agent_commands, command);

    // if none found, throw error
    if (!command_ptr) {
        throw Error("Unrecognized command!");
    }

    return command_ptr;
}

Controller::Controller_fp_t Controller::get_view_program_command(const string& command) {
    // try to find program command
    Controller_fp_t command_ptr = get_command_helper(m_program_commands, command);

    // if none found, try to find view command
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
    shared_ptr<View> view = make_shared<View>();
    Model::get_instance()->attach(view);

    init_commands(); // Fill command containers

    string first_word;

    // main program loop, exited when user enters "quit" command
    while (true) {
        try {
            cout << "\nTime " << Model::get_instance()->get_time() << ": Enter command: ";
            cin >> first_word;

            // Check if user wants to quit
            if (first_word == "quit") {
                break; // break out of main program loop
            }

            // Check if first word is name of an Agent and set the Agent ptr
            // then execute an Agent command
            if (Model::get_instance()->is_agent_present(first_word)) {
                shared_ptr<Agent> agent_ptr = Model::get_instance()->get_agent_ptr(first_word);
                if (!agent_ptr->is_alive()) {
                    throw Error("Agent is not alive!");
                }

                // Find Agent command, throws Error if none found
                Controller_agent_fp_t agent_command_ptr = get_agent_command();
                // Execute Agent command
                (this->*(agent_command_ptr))(agent_ptr);
            }
            // Otherwise check if user input a different command and execute it
            else {
                // Try to find a view or program-wide command, Error thrown
                // if first_word is not a valid command
                Controller_fp_t command_ptr = get_view_program_command(first_word);
                // Execute command
                (this->*(command_ptr))();
            }

        } // End try-block
        catch (exception& e) {
            cout << e.what() << endl;

            // If we've reached EOF allow program to exit normally
            if (cin.eof()) break;

            discard_rest_of_line(cin);
        }
        catch (...) {
            cout << "Unknown exception caught!" << endl;
        }
    } // End main program loop

    // Tear down View
    Model::get_instance()->detach(view);

    cout << "Done" << endl;
}

// View commands from spec
void Controller::view_default_command() {
    Model::get_instance()->apply_to_all_views([](View& v) { v.set_defaults(); });
}

void set_view_size_helper(int new_size, View& view) {
    view.set_size(new_size);
}

void set_view_scale_helper(double new_scale, View& view) {
    view.set_scale(new_scale);
}

void set_view_origin_helper(Point new_origin, View& view) {
    view.set_origin(new_origin);
}

void Controller::view_size_command() {
    int new_size = read_int();
    Model::get_instance()->apply_to_all_views_arg(new_size, &set_view_size_helper);
}

void Controller::view_zoom_command() {
    double new_scale = read_double();
    Model::get_instance()->apply_to_all_views_arg(new_scale, &set_view_scale_helper);
}

static Point read_point() {
    double new_x = read_double();
    double new_y = read_double();
    return Point(new_x, new_y);
}

void Controller::view_pan_command() {
    Point new_origin = read_point();
    Model::get_instance()->apply_to_all_views_arg(new_origin, &set_view_origin_helper);
}

void Controller::agent_move_command(shared_ptr<Agent> agent_ptr) {
    Point move_pt = read_point();
    agent_ptr->move_to(move_pt);
}

static shared_ptr<Structure> read_structure_from_input() {
    string structure_name;
    cin >> structure_name;
    return Model::get_instance()->get_structure_ptr(structure_name);
}

void Controller::agent_work_command(shared_ptr<Agent> agent_ptr) {
    // Read and find these Structures, throws Error if either is not found
    shared_ptr<Structure> source_ptr = read_structure_from_input();
    shared_ptr<Structure> destination_ptr = read_structure_from_input();

    assert(source_ptr);
    assert(destination_ptr);

    // Tell Agent to work, throws Error if Agent cannot work
    agent_ptr->start_working(source_ptr, destination_ptr);
}

void Controller::agent_attack_command(shared_ptr<Agent> agent_ptr) {
    // read name for target to attack
    string target_name;
    cin >> target_name;

    // Find target Agent, throws Error if Agent not found
    shared_ptr<Agent> target_ptr = Model::get_instance()->get_agent_ptr(target_name);
    assert(target_ptr);

    // Attack target if possible, throws Error if Agent cannot attack
    agent_ptr->start_attacking(target_ptr);
}

void Controller::agent_stop_command(shared_ptr<Agent> agent_ptr) {
    agent_ptr->stop();
}

void Controller::status_command() {
    Model::get_instance()->describe();
}

void Controller::show_command() {
    Model::get_instance()->apply_to_all_views([](View& v) { v.draw(); });
}

void Controller::go_command() {
    Model::get_instance()->update();
}

static void read_name(string& name) {
    cin >> name;

    // Check if input name is a valid object name
    if (name.length() < 2 || !string_is_alnum(name) ||
        Model::get_instance()->is_name_in_use(name))
    {
        throw Error("Invalid name for new object!");
    }
}

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
    shared_ptr<Structure> new_structure = create_structure(info.name,
                                                info.type,
                                                info.start_pt);

    // Add new Structure to Model
    Model::get_instance()->add_structure(new_structure);
}

void Controller::train_command() {
    New_obj_info info;
    read_new_obj_info(info);

    // create new Structure if information read in is valid
    shared_ptr<Agent> new_agent = create_agent(info.name, info.type, info.start_pt);

    // Add new Structure to Model
    Model::get_instance()->add_agent(new_agent);
}
