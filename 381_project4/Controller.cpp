#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Utility.h"
#include <iostream>

using namespace std;

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

void Controller::init_commands() {
    m_view_commands["default"] = &view_default_command;
    m_view_commands["size"] = &view_size_command;
    m_view_commands["zoom"] = &view_zoom_command;
    m_view_commands["pan"] = &view_pan_command;

    m_agent_commands["move"] = &agent_move_command;
    m_agent_commands["work"] = &agent_work_command;
    m_agent_commands["attack"] = &agent_attack_command;
    m_agent_commands["stop"] = &agent_stop_command;

    m_program_commands["status"] = &status_command;
    m_program_commands["show"] = &show_command;
    m_program_commands["go"] = &go_command;
    m_program_commands["build"] = &build_command;
    m_program_commands["train"] = &train_command;
}

void Controller::run() {
    View view;
    g_Model_ptr->attach(&view);

    init_commands();


    g_Model_ptr->detach(&view);
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
