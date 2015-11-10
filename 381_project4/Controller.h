#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>

/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

class Controller {
public:
    Controller();

    // create View object, run the program by acccepting user commands, then destroy View object
    void run();

private:
    // View commands from spec
    void view_default_command();
    void view_size_command();
    void view_zoom_command();
    void view_pan_command();

    // Agent commands from spec
    void agent_move_command();
    void agent_work_command();
    void agent_attack_command();
    void agent_stop_command();

    // Whole-program commands from spec
    void status_command();
    void show_command();
    void go_command();
    void build_command();
    void train_command();

    void init_commands();

    std::map<std::string, void(Controller::*)()> m_view_commands;
    std::map<std::string, void(Controller::*)()> m_agent_commands;
    std::map<std::string, void(Controller::*)()> m_program_commands;
};

#endif // CONTROLLER_H
