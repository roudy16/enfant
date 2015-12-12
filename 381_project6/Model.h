#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

// Forward declarations
class Model;
class View;
class Sim_object;
class Structure;
class Agent;
class Group;
struct Point;

/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Structures and Agents there are, but it does not
know about any of their derived classes, nor which Agents are of what kind of Agent. 
It has facilities for looking up objects by name, and removing Agents.  When
created, it creates an initial group of Structures and Agents using the Structure_factory
and Agent_factory.
Finally, it keeps the system's time.

Controller tells Model what to do; Model in turn tells the objects what do, and
when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
Model also provides facilities for looking up objects given their name.

Notice how only the Standard Library headers need to be included - reduced coupling!

Implemented as a Singleton
*/


class Model {

public:
    // disallow copy/move construction or assignment
    Model(const Model&) = delete;
    Model& operator= (const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator= (Model&&) = delete;

    // return pointer to the Model object
    static Model* get_instance();
    
    // return the current time
    int get_time() {return m_time;}

    // is name already in use for either agent or structure?
    // return true if the name matches the name of an existing agent or structure
    bool is_name_in_use(const std::string& name) const;

    std::shared_ptr<Sim_object> get_obj_ptr(const std::string& name) const;

    // is there a structure with this name?
    bool is_structure_present(const std::string& name) const;
    // add a new structure; assumes none with the same name
    void add_structure(std::shared_ptr<Structure>);
    // will throw Error("Structure not found!") if no structure of that name
    std::shared_ptr<Structure> get_structure_ptr(const std::string& name) const;
    // TODO needed?
    // returns pointer to Structure with name if it exists, empty pointer otherwise
    std::shared_ptr<Structure> find_structure(const std::string& name) const;
    // returns pointer to closest Structure to passed in object, empty pointer otherwise
    std::shared_ptr<Structure> get_closest_structure_to_obj(std::shared_ptr<Sim_object>);

    // is there an agent with this name?
    bool is_agent_present(const std::string& name) const;
    // add a new agent; assumes none with the same name
    void add_agent(std::shared_ptr<Agent> agent_ptr);
    // remove Agent from all containers, agent_ptr must not be nullptr
    void remove_agent(std::shared_ptr<Agent> agent_ptr);
    // will throw Error("Agent not found!") if no agent of that name
    std::shared_ptr<Agent> get_agent_ptr(const std::string& name) const;
    // returns pointer to Agent with name if it exists, empty pointer otherwise
    std::shared_ptr<Agent> find_agent(const std::string& name) const;
    // returns pointer to closest Agent to passed in object, empty pointer otherwise
    std::shared_ptr<Agent> get_closest_agent_to_obj(std::shared_ptr<Sim_object>);
    // returns pointer to closest Agent that does not share a Group with passed in agent
    std::shared_ptr<Agent> get_closest_hostile_agent(std::shared_ptr<Agent>);

    // is there a group with this name?
    bool is_group_present(const std::string& name) const;
    void add_group(std::shared_ptr<Group> group_ptr);
    void remove_group(const std::string& name);
    // will throw Error("Group not found!") if no Group of that name
    std::shared_ptr<Group> get_group_ptr(const std::string& name) const;
    // returns pointer to Group with name if it exists, empty pointer otherwise
    std::shared_ptr<Group> find_group(const std::string& name) const;

    // tell all objects to describe themselves to the console
    void describe() const;
    // increment the time, and tell all objects to update themselves
    void update();

    /* View services */
    // Attaching a View adds it to the container and causes it to be updated
    // with all current objects'location (or other state information.
    void attach(std::shared_ptr<View>);
    // Detach the View by discarding the supplied pointer from the container of Views
    // - no updates sent to it thereafter.
    void detach(std::shared_ptr<View>);
    // notify the views about an object's location
    void notify_location(const std::string& name, const Point& location);
    // notify the views that an object is now gone
    void notify_gone(const std::string& name);
    // notify the views of an Agent's health
    void notify_health(const std::string& name, double health);
    // notify the views of a Structure's food amount
    void notify_amount(const std::string& name, double amount);
    // notify the views to draw themselves
    void notify_draw();
    // TODO remove this
    // Returns shared_ptr to View with name, returns empty ptr otherwise
    std::shared_ptr<View> find_view(const std::string& name);

    // class used to deallocate Model
    friend class Model_destroyer;

private:
    using Sim_objs_t = std::map < const std::string, std::shared_ptr<Sim_object> > ;

    // create the initial objects
    Model();
    // destroy all objects
    ~Model();
    // Initialize the Model, not called in ctor to prevent recursive initialization
    void init();

    // Helper template function for finding closest objects to another
    // when used to search a map container of shared_ptrs to Sim_objects function will
    // return a shared_ptr to the closest object to obj_ptr
    template <typename C>
    typename C::mapped_type get_closest_helper(C&, std::shared_ptr<Sim_object>);

    // Returns true if both Agents are present in the same group
    bool agents_share_group(std::shared_ptr<Agent> agent_a, std::shared_ptr<Agent> agent_b) const;

    static Model* mp_instance; // pointer to single instance of Model

    Sim_objs_t                                               m_sim_objs;
    std::map<const std::string, std::shared_ptr<Agent>>      m_agents;
    std::map<const std::string, std::shared_ptr<Structure>>  m_structures;
    std::vector<std::shared_ptr<Group>>                      m_groups;
    std::vector<std::shared_ptr<View>>                       m_views;

    int m_time;
};

#endif // MODEL_H
