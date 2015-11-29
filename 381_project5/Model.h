#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include <vector>
#include <memory>

// Forward declarations
class Model;
class View;
class Sim_object;
class Structure;
class Agent;
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

    // is there a structure with this name?
    bool is_structure_present(const std::string& name) const;
    // add a new structure; assumes none with the same name
    void add_structure(std::shared_ptr<Structure>);
    // will throw Error("Structure not found!") if no structure of that name
    std::shared_ptr<Structure> get_structure_ptr(const std::string& name) const;

    // is there an agent with this name?
    bool is_agent_present(const std::string& name) const;
    // add a new agent; assumes none with the same name
    void add_agent(std::shared_ptr<Agent> agent_ptr);
    // remove Agent from all containers, agent_ptr must not be nullptr
    void remove_agent(std::shared_ptr<Agent> agent_ptr);
    // will throw Error("Agent not found!") if no agent of that name
    std::shared_ptr<Agent> get_agent_ptr(const std::string& name) const;
    
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
    void notify_location(const std::string& name, Point location);
    // notify the views that an object is now gone
    void notify_gone(const std::string& name);
    // notify the views to draw themselves
    void notify_draw();
    // Returns shared_ptr to View with name, returns empty ptr otherwise
    std::shared_ptr<View> find_view(const std::string& name);

    // class used to deallocate Model
    friend class Model_destroyer;

private:
    using Views_t = std::vector<std::shared_ptr<View>>;

    // create the initial objects
    Model();
    // destroy all objects
    ~Model();
    // Initialize the Model, not called in ctor to prevent recursive initialization
    void init();

    // Comparator for sorting containers
    template<typename T>
    struct Obj_ptr_comp {
        bool operator()(const T lhs, const T rhs) {
            return lhs->get_name() < rhs->get_name();
        }
    };

    static Model* mp_instance; // pointer to single instance of Model

    std::map<const std::string, std::shared_ptr<Sim_object>> m_sim_objs;
    std::map<const std::string, std::shared_ptr<Agent>> m_agents;
    std::map<const std::string, std::shared_ptr<Structure>> m_structures;
    Views_t m_views;
    int m_time;
};

#endif // MODEL_H
