#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Views.h"
#include "Utility.h"
#include "Geometry.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Agent.h"
#include "Structure.h"
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <utility>
#include <cassert>

using std::string;
using std::cin; using std::cout; using std::endl;
using std::shared_ptr; using std::weak_ptr; using std::static_pointer_cast; using std::make_shared;
using std::exception; using std::runtime_error;
using std::numeric_limits;
using std::streamsize;
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

        m_program_commands["open"] = &Controller::open_command;
        m_program_commands["close"] = &Controller::close_command;
        m_program_commands["status"] = &Controller::status_command;
        m_program_commands["show"] = &Controller::show_command;
        m_program_commands["go"] = &Controller::go_command;
        m_program_commands["build"] = &Controller::build_command;
        m_program_commands["train"] = &Controller::train_command;
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
    read_in_string(target_name);

    // Find target Agent, throws Error if Agent not found
    shared_ptr<Agent> target_ptr = Model::get_instance()->get_agent_ptr(target_name);
    assert(target_ptr);

    // Attack target if possible, throws Error if Agent cannot attack
    agent_ptr->start_attacking(target_ptr);
}

void Controller::agent_stop_command(shared_ptr<Agent> agent_ptr) {
    // Have Agent stop everything it is doing
    agent_ptr->stop();
}

void Controller::status_command() {
    // tell all objects to describe themselves to the console
    Model::get_instance()->describe();
}

// Attempt to open and attach a new View to the Model
void Controller::open_command() {
    string view_name;
    read_in_string(view_name);

    // Check if there is already a View of this name open
    shared_ptr<View> view_ptr = Model::get_instance()->find_view(view_name);
    if (view_ptr) {
        throw Error("View of that name already open!");
    }

    // Check what type of View user wants
    if (view_name == "map") {
        // create a World map that shows a large area of the world
        shared_ptr<World_map> world_map_ptr = make_shared<World_map>("map");
        Model::get_instance()->attach(static_pointer_cast<View>(world_map_ptr));
        mp_map_view = weak_ptr<World_map>(world_map_ptr);
    }
    else if (view_name == "health") {
        // create a health status view that shows the health of Agents
        auto health_status_ptr = make_shared<Health_status>();
        Model::get_instance()->attach(static_pointer_cast<View>(health_status_ptr));
    }
    else if (view_name == "amounts") {
        // create an amount status view that shows the food amounts of Sim_objects
        auto amounts_status_ptr = make_shared<Amount_status>();
        Model::get_instance()->attach(static_pointer_cast<View>(amounts_status_ptr));

    }
    else {
        // Create a local map view centered on a Sim_object that matches the input view_name,
        // throw an Error if no such object exists
        shared_ptr<Sim_object> obj_ptr = Model::get_instance()->get_obj_ptr(view_name);

        if (!obj_ptr) {
            throw Error("No object of that name!");
        }

        auto local_map_ptr = make_shared<Local_map>(obj_ptr->get_name());
        Model::get_instance()->attach(static_pointer_cast<View>(local_map_ptr));
    }
}

// Attempt to close and detach a View from Model
void Controller::close_command() {
    string view_name;
    read_in_string(view_name);

    // Check if there is an open View that matches input name
    shared_ptr<View> view_ptr = Model::get_instance()->find_view(view_name);
    if (!view_ptr) {
        throw Error("No view of that name is open!");
    }

    Model::get_instance()->detach(view_ptr);
}

// Draw all Views
void Controller::show_command() {
    Model::get_instance()->notify_draw();
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
