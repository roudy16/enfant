#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "World_map.h"
#include "Utility.h"
#include "Geometry.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "View_factory.h"
#include "Agent.h"
#include "Structure.h"
#include "Group.h"
#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <utility>
#include <cassert>

using std::string;
using std::vector;
using std::cin; using std::cout; using std::endl;
using std::shared_ptr; using std::weak_ptr; using std::make_shared;
using std::exception; using std::runtime_error;
using std::numeric_limits;
using std::streamsize;
using std::for_each; using std::find_if;
using namespace std::placeholders;

// Forward declaration
struct New_obj_info;

// Helpers
static Point read_point();
static void read_in_string(string& str);
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

// Initialize mapped containers that hold function pointers to user command logic
void Controller::init_commands() {
    try {
        m_view_commands["default"] = &Controller::view_default_command;
        m_view_commands["size"] = &Controller::view_size_command;
        m_view_commands["zoom"] = &Controller::view_zoom_command;
        m_view_commands["pan"] = &Controller::view_pan_command;

        m_agent_commands["move"] = &Controller::agent_move_command;
        m_agent_commands["work"] = &Controller::agent_work_command;
        m_agent_commands["attack"] = &Controller::agent_attack_command;
        m_agent_commands["stop"] = &Controller::agent_stop_command;

        m_group_commands["disband"] = &Controller::group_disband_command;
        m_group_commands["add"] = &Controller::group_add_command;
        m_group_commands["remove"] = &Controller::group_remove_command;
        m_group_commands["move"] = &Controller::group_move_command;
        m_group_commands["stop"] = &Controller::group_stop_command;
        m_group_commands["attack"] = &Controller::group_attack_command;
        m_group_commands["work"] = &Controller::group_work_command;

        m_program_commands["open"] = &Controller::open_command;
        m_program_commands["close"] = &Controller::close_command;
        m_program_commands["status"] = &Controller::status_command;
        m_program_commands["show"] = &Controller::show_command;
        m_program_commands["go"] = &Controller::go_command;
        m_program_commands["build"] = &Controller::build_command;
        m_program_commands["train"] = &Controller::train_command;
        m_program_commands["form_group"] = &Controller::create_group_command;
    }
    catch (...) {
        throw runtime_error("Error detected in Controller::init_commands");
    }
}

static void read_in_string(string& str) {
    cin >> str;

    if (!cin.good()) {
        throw runtime_error("Failed to read string from input");
    }
}

static void discard_rest_of_line(std::istream& is) {
    is.clear();
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

Controller::Controller_agent_fp_t Controller::get_agent_command() {
    string command;
    read_in_string(command);

    // Try to find Agent command function
    Controller_agent_fp_t command_ptr = get_command_helper(m_agent_commands, command);

    // if none found, throw error
    if (!command_ptr) {
        throw Error("Unrecognized command!");
    }

    return command_ptr;
}

Controller::Controller_group_fp_t Controller::get_group_command() {
    string command;
    read_in_string(command);

    // Try to find Agent command function
    Controller_group_fp_t command_ptr = get_command_helper(m_group_commands, command);

    // if none found, throw error
    if (!command_ptr) {
        throw Error("Unrecognized group command!");
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
    init_commands(); // Fill command containers

    // main program loop, exited when user enters "quit" command
    while (true) {
        try {
            cout << "\nTime " << Model::get_instance()->get_time() << ": Enter command: ";
            string first_word;
            read_in_string(first_word);

            // Check if user wants to quit
            if (first_word == "quit") {
                break; // break out of main program loop
            }

            // Check if first word is name of an Agent and set the Agent ptr
            // then execute an Agent command
            shared_ptr<Agent> agent_ptr = Model::get_instance()->find_agent(first_word);
            if (agent_ptr) {
                assert(agent_ptr->is_alive());

                // Find Agent command, throws Error if none found
                Controller_agent_fp_t agent_command_ptr = get_agent_command();
                // Execute Agent command
                (this->*agent_command_ptr)(agent_ptr);
                continue;
            }

            // Check if first word is name of an active Group, if it is then
            // get the Group and execute the command
            shared_ptr<Group> group_ptr = Model::get_instance()->find_group(first_word);
            if (group_ptr) {
                // Get group command funciton pointer, throws Error if unrecognized command
                Controller_group_fp_t group_command_ptr = get_group_command();

                (this->*group_command_ptr)(group_ptr);
                continue;
            }

            // Otherwise check if user input a different command and execute it
            // Try to find a view or program-wide command, Error thrown
            // if first_word is not a valid command
            Controller_fp_t command_ptr = get_view_program_command(first_word);
            // Execute command
            (this->*(command_ptr))();
        } // End try-block
        catch (exception& e) {
            cout << e.what() << endl;
            discard_rest_of_line(cin);
        }
        catch (...) {
            cout << "Unknown exception caught!" << endl;
            break;
        }
    } // End main program loop

    cout << "Done" << endl;
}

// Return a shared_ptr to the open map view, throw an Error if no map view open
shared_ptr<World_map> Controller::get_map_view() {
    if (mp_map_view.expired()) {
        throw Error("No map view is open!");
    }

    return mp_map_view.lock();
}

// View commands from spec
void Controller::view_default_command() {
    get_map_view()->set_defaults();
}

void Controller::view_size_command() {
    auto map_view_ptr = get_map_view();
    int new_size = read_int();
    map_view_ptr->set_size(new_size);
}

void Controller::view_zoom_command() {
    auto map_view_ptr = get_map_view();
    double new_scale = read_double();
    map_view_ptr->set_scale(new_scale);
}

static Point read_point() {
    double new_x = read_double();
    double new_y = read_double();
    return Point(new_x, new_y);
}

void Controller::view_pan_command() {
    auto map_view_ptr = get_map_view();
    Point new_origin = read_point();
    map_view_ptr->set_origin(new_origin);
}

void Controller::agent_move_command(shared_ptr<Agent> agent_ptr) {
    Point move_pt = read_point();
    agent_ptr->move_to(move_pt);
}

static shared_ptr<Structure> read_structure_from_input() {
    string structure_name;
    read_in_string(structure_name);

    shared_ptr<Structure> structure_ptr = Model::get_instance()->find_structure(structure_name);
    if (!structure_ptr) {
        throw Error("Structure not found!");
    }

    return structure_ptr;
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

static shared_ptr<Agent> read_in_attack_target() {
    // read name for target to attack
    string target_name;
    read_in_string(target_name);

    // Find target Agent, throws Error if Agent not found
    shared_ptr<Agent> target_ptr = Model::get_instance()->find_agent(target_name);

    if (!target_ptr) {
        throw Error("Agent not found!");
    }

    return target_ptr;
}

void Controller::agent_attack_command(shared_ptr<Agent> agent_ptr) {
    shared_ptr<Agent> target_ptr = read_in_attack_target();
    // Attack target if possible, throws Error if Agent cannot attack
    agent_ptr->start_attacking(target_ptr);
}

void Controller::agent_stop_command(shared_ptr<Agent> agent_ptr) {
    // Have Agent stop everything it is doing
    agent_ptr->stop();
}

// Group commands
void Controller::group_disband_command(shared_ptr<Group> group_ptr) {
    group_ptr->disband();
    Model::get_instance()->remove_group(group_ptr->get_name());
}

// Provides basic framework for adding/removing to/from groups
// Function pointers passed in should be either both Add functions or
// both Remove functions for Group.
static void group_add_remove_helper(shared_ptr<Group> group_ptr,
                                    void(Group::*agent_fp)(shared_ptr<Agent>),
                                    void(Group::*group_fp)(shared_ptr<Group>))
{
    string member_name;
    read_in_string(member_name);

    // Find Agent and add/remove it to/from group
    shared_ptr<Agent> agent_ptr = Model::get_instance()->find_agent(member_name);
    if (agent_ptr) {
        ((*group_ptr).*agent_fp)(agent_ptr);
        return;
    }

    // If no Agent with member name was found attempt to find a group with that name
    shared_ptr<Group> other_group_ptr = Model::get_instance()->find_group(member_name);

    // Throw Error if no Group was found with member name either
    if (!other_group_ptr) {
        throw Error("No Agent or Group found with that name!");
    }

    // Add/Remove the found group to/from the passed in group
    ((*group_ptr).*group_fp)(group_ptr);
}

void Controller::group_add_command(shared_ptr<Group> group_ptr) {
    group_add_remove_helper(group_ptr, &Group::add_agent, &Group::add_group);
}

void Controller::group_remove_command(shared_ptr<Group> group_ptr) {
    group_add_remove_helper(group_ptr, &Group::remove_agent, &Group::remove_group);
}

void Controller::group_move_command(shared_ptr<Group> group_ptr) {
    Point destination = read_point();
    group_ptr->move(destination);
}

void Controller::group_stop_command(shared_ptr<Group> group_ptr) {
    group_ptr->stop();
}

void Controller::group_attack_command(shared_ptr<Group> group_ptr) {
    shared_ptr<Agent> target_ptr = read_in_attack_target();
    group_ptr->attack(target_ptr);
}

void Controller::group_work_command(shared_ptr<Group> group_ptr) {
    shared_ptr<Structure> source = read_structure_from_input();
    shared_ptr<Structure> destination = read_structure_from_input();

    group_ptr->work(source, destination);
}

void Controller::status_command() {
    // tell all objects to describe themselves to the console
    Model::get_instance()->describe();
}

class View_find_pred {
public:
    View_find_pred(const string& name) : m_name(name) {}

    bool operator()(shared_ptr<View> v) const { return v->get_name() == m_name; }

private:
    string m_name;
};

// Attempt to open and attach a new View to the Model
void Controller::open_command() {
    string view_name;
    read_in_string(view_name);

    // Check if there is already a View of this name open
    auto iter = find_if(m_views.begin(), m_views.end(), View_find_pred(view_name));
    if (iter != m_views.end()) {
        throw Error("View of that name already open!");
    }

    View_factory_return ret_val = create_view(view_name);

    // If newly created map is the world map then remember this with weak_ptr
    if (!ret_val.world_map_ptr.expired()) {
        mp_map_view = ret_val.world_map_ptr;
    }

    m_views.push_back(ret_val.view_ptr);
    Model::get_instance()->attach(ret_val.view_ptr);
}

// Attempt to close and detach a View from Model
void Controller::close_command() {
    string view_name;
    read_in_string(view_name);

    // Check if there is an open View that matches input name
    auto iter = find_if(m_views.begin(), m_views.end(), View_find_pred(view_name));

    if (iter == m_views.end()) {
        throw Error("No view of that name is open!");
    }

    m_views.erase(iter);
    Model::get_instance()->detach(*iter);
}

// Draw all Views
void Controller::show_command() {
    for_each(m_views.begin(), m_views.end(), [](shared_ptr<View>& v){ v->draw(); });
}

// Update all Sim_objects
void Controller::go_command() {
    Model::get_instance()->update();
}

// Data for creating a new Sim_object
struct New_obj_info {
    string name;
    string type;
    Point start_pt;
};

// Read data for creating new Sim_object from input
static void read_new_obj_info(New_obj_info& info) {
    read_in_string(info.name);

    // Check if input name is a valid object name
    if (info.name.length() < 2 || !string_is_alnum(info.name) ||
        Model::get_instance()->is_name_in_use(info.name))
    {
        throw Error("Invalid name for new object!");
    }

    // read in type of Structure to create, no validation yet
    read_in_string(info.type);

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

void Controller::create_group_command() {
    string name;
    read_in_string(name);

    // Check if input name is a valid group name
    // Must be at least 1 character in length, contain only alphanumeric 
    // characters and not already be in use by another Agent, Structure
    // or Group
    if (name.length() < 1 || !string_is_alnum(name) ||
        Model::get_instance()->is_name_in_use(name))
    {
        throw Error("Invalid name for new group!");
    }

    // Create new group and add have Model at it to its group container
    shared_ptr<Group> new_group = make_shared<Group>(name);
    Model::get_instance()->add_group(new_group);
}

