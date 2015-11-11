#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>

class Agent;

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
    using Controller_fp_t = void(Controller::*)();
    using Command_map_t = std::map<std::string, Controller_fp_t>;

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

    // Returns function pointer to associated command if it exists, 
    // returns nullptr otherwise
    Controller_fp_t get_view_program_command(const std::string& command);
    // get_agent_command additionally reads in a string
    Controller_fp_t get_agent_command();
    Controller_fp_t get_command_helper(Command_map_t& commands,
                                       const std::string& command);

    Command_map_t m_view_commands;
    Command_map_t m_agent_commands;
    Command_map_t m_program_commands;

    Agent* mp_current_agent;
};

#endif // CONTROLLER_H
