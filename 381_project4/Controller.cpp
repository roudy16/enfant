#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Utility.h"
#include <iostream>
#include <exception>
#include <algorithm>
#include <utility>
#include <ctype.h>

using namespace std;

Controller::Controller() : mp_current_agent(nullptr)
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

static string read_name() {
    string name;
    cin >> name;

    // Check if input name is a valid object name
    if (name.length() < 2 || !string_is_alnum(name) ||
        g_Model_ptr->is_name_in_use(name))
    {
        throw Error("Invalid name for new object!");
    }

    return move(name);
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

    return get_command_helper(m_agent_commands, command);
}

Controller::Controller_fp_t Controller::get_view_program_command(const string& command) {
    // try to find program command
    Controller_fp_t command_ptr = get_command_helper(m_program_commands, command);

    // if program command not found, try to find view command
    if (!command_ptr) {
        command_ptr = get_command_helper(m_view_commands, command);
    }

    return command_ptr;
}

void Controller::run() {
    View view;
    g_Model_ptr->attach(&view);

    init_commands();

    string first_word;

    Controller_fp_t command_ptr = nullptr;

    // main program loop, exitted when user enters "quit" command
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

                // check if valid Agent command found
                command_ptr = get_agent_command();
                if (!command_ptr) {
                    throw Error("Unrecognized command!");
                }

                // Execute Agent command
                (this->*(command_ptr))();
            }
            // Otherwise check if user input a different command and execute it
            else {
                // Try to find a view or program-wide command
                command_ptr = get_view_program_command(first_word);

                // if none found, throw error
                if (!command_ptr) {
                    throw Error("Unrecognized command!");
                }

                // Execute Agent command
                (this->*(command_ptr))();
            }

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

    g_Model_ptr->detach(&view);

    cout << "Done" << endl;
}

// View commands from spec
void Controller::view_default_command() {

}

void Controller::view_size_command() {

}

void Controller::view_zoom_command() {

}

void Controller::view_pan_command() {

}

// Agent commands from spec
void Controller::agent_move_command() {

}

void Controller::agent_work_command() {

}

void Controller::agent_attack_command() {

}

void Controller::agent_stop_command() {

}

// Whole-program commands from spec
void Controller::status_command() {

}

void Controller::show_command() {

}

void Controller::go_command() {

}

void Controller::build_command() {

}

void Controller::train_command() {

}
