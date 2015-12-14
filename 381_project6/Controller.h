#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <vector>
#include <string>
#include <memory>

class Agent;
class Group;
class View;
class World_map;

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
    void agent_move_command(std::shared_ptr<Agent>);
    void agent_work_command(std::shared_ptr<Agent>);
    void agent_attack_command(std::shared_ptr<Agent>);
    void agent_stop_command(std::shared_ptr<Agent>);

    // Whole-program commands from spec
    void open_command();
    void close_command();
    void status_command();
    void show_command();
    void go_command();
    void build_command();
    void train_command();
    void create_group_command();

    // Group commands
    void group_disband_command(std::shared_ptr<Group>);
    void group_add_command(std::shared_ptr<Group>);
    void group_remove_command(std::shared_ptr<Group>);
    void group_move_command(std::shared_ptr<Group>);
    void group_stop_command(std::shared_ptr<Group>);
    void group_attack_command(std::shared_ptr<Group>);
    void group_work_command(std::shared_ptr<Group>);

    // helper to initialize command containers with string to 
    // function pointer mappings, returns true on success
    bool init_commands();

    using Controller_fp_t = void(Controller::*)();
    using Controller_agent_fp_t = void(Controller::*)(std::shared_ptr<Agent>);
    using Controller_group_fp_t = void(Controller::*)(std::shared_ptr<Group>);
    using Command_map_t = std::map<std::string, Controller_fp_t>;
    using Agent_command_map_t = std::map<std::string, Controller_agent_fp_t>;
    using Group_command_map_t = std::map<std::string, Controller_group_fp_t>;

    // Returns function pointer to associated command if it exists, 
    // returns nullptr otherwise
    Controller_fp_t get_view_program_command(const std::string& command);
    // get_agent_command additionally reads in a string
    Controller_agent_fp_t get_agent_command();
    // Returns function ptr to group command associated with next input string
    Controller_group_fp_t get_group_command();

    // Returns shared_ptr to the map view if one exists, otherwise
    // throws an Error
    std::shared_ptr<World_map> get_map_view();

    // Containers for user command function pointers
    Agent_command_map_t       m_agent_commands;
    Group_command_map_t       m_group_commands;
    Command_map_t             m_view_commands;
    Command_map_t             m_program_commands;

    // Container to hold the Views 
    // member that holds ptr to World map view when it is open
    std::vector<std::shared_ptr<View>> m_views;
    std::weak_ptr<World_map>           mp_map_view;

    template<typename C>
    typename C::mapped_type get_command_helper(C& commands, const std::string& command);

    // disallow copy/move construction or assignment
    Controller(const Controller&) = delete;
    Controller& operator= (const Controller&)  = delete;
    Controller(Controller&&) = delete;
    Controller& operator= (Controller&&) = delete;
};

template<typename C>
typename C::mapped_type Controller::get_command_helper(C& commands, const std::string& command) {
    typename C::iterator iter = commands.find(command);

    if (iter == commands.end()) {
        return nullptr;
    }

    return iter->second;
}

#endif // CONTROLLER_H
