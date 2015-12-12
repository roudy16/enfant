#include "Model.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Agent.h"
#include "Group.h"
#include "Utility.h"
#include "View.h"
#include <algorithm>
#include <cassert>

using std::string;
using std::shared_ptr; using std::static_pointer_cast;
using std::min_element; using std::find; using std::for_each;

// class used to deallocate Model
class Model_destroyer {
public:
    ~Model_destroyer() {
        delete Model::mp_instance;
    }
};

// global object that deallocates Model singleton memory when program finishes
static Model_destroyer the_destroyer;

Model* Model::mp_instance = nullptr;

Model* Model::get_instance() {
    if (!mp_instance) {
        mp_instance = new Model();
        mp_instance->init();
    }
    return mp_instance;
}

Model::Model() : m_time(0) 
{
}

Model::~Model() {
}

void Model::init() {
    // Starting Structures
    add_structure(create_structure("Rivendale", "Farm", Point(10., 10.)));
    add_structure(create_structure("Sunnybrook", "Farm", Point(0., 30.)));
    add_structure(create_structure("Shire", "Town_Hall", Point(20., 20.)));
    add_structure(create_structure("Paduca", "Town_Hall", Point(30., 30.)));

    // Starting Agents
    add_agent(create_agent("Pippin", "Peasant", Point(5., 10.)));
    add_agent(create_agent("Merry", "Peasant", Point(0., 25.)));
    add_agent(create_agent("Zug", "Soldier", Point(20., 30.)));
    add_agent(create_agent("Bug", "Soldier", Point(15., 20.)));
    add_agent(create_agent("Iriel", "Archer", Point(20., 38.)));
    add_agent(create_agent("Randalf", "Mage", Point(15., 30.)));
}

// is name already in use for another Agent, Structure, or Group?
// return true if the name matches the name of an existing Agent, Structure
// or Group
bool Model::is_name_in_use(const string& name) const {
    return is_structure_present(name) ||
           is_agent_present(name) ||
           is_group_present(name);
}

shared_ptr<Sim_object> Model::get_obj_ptr(const string& name) const {
    auto iter = m_sim_objs.find(name);

    // if not found return an empty ptr
    if (iter == m_sim_objs.end()) {
        return shared_ptr<Sim_object>();
    }

    return iter->second;
}

bool Model::is_structure_present(const string& name) const {
    auto iter = m_structures.find(name);

    // return false if structure with name not found, return true otherwise
    return iter != m_structures.end();
}

// add a new structure; assumes none with the same name
void Model::add_structure(shared_ptr<Structure> new_structure_ptr) {
    // Add Structure to Sim_objects container as a Sim_object*
    m_sim_objs[new_structure_ptr->get_name()] = static_pointer_cast<Sim_object>(new_structure_ptr);

    // Add Structure to Structures container as a shared_ptr<Structure>
    m_structures[new_structure_ptr->get_name()] = new_structure_ptr;
    new_structure_ptr->broadcast_current_state();
}

shared_ptr<Structure> Model::get_structure_ptr(const string& name) const {
    auto iter = m_structures.find(name);

    // throw Error if structure not found
    if (iter == m_structures.end()) {
        throw Error("Structure not found!");
    }

    return iter->second;
}

// Comparator for finding closest object to another object. The Comparator is 
// initialized with the unique name of an object that we want to find the closest 
// other object to. When used to search a container of objects, object that evaluates
// as least is the object that is closest to the object used to init. In the event
// of a tie the object with the lesser lexicographical name is least
class Closest_to_obj {
public:
    Closest_to_obj(shared_ptr<Sim_object> obj_ptr)
        : m_location(obj_ptr->get_location()), m_name(obj_ptr->get_name())
    {
    }

    // Compare distances and names of objects
    // Note: this template only works with std::map iterators that point
    // to Sim_objects or objects derived some Sim_object
    template <typename T>
    bool operator()(T& lhs, T& rhs) const {
        // Check if either argument is the object used to init
        // Object used to init always evaluates greater than any other
        if (lhs.second->get_name() == m_name) {
            return false;
        }
        if (rhs.second->get_name() == m_name) {
            return true;
        }

        // get distances from args to init object
        double dist_to_lhs = cartesian_distance(m_location, lhs.second->get_location());
        double dist_to_rhs = cartesian_distance(m_location, rhs.second->get_location());

        // resolves distance ties with name comparison
        if (dist_to_lhs == dist_to_rhs) {
            return lhs.second->get_name() < rhs.second->get_name();
        }

        // return true if lhs is closer than rhs
        return dist_to_lhs < dist_to_rhs;
    }

private:
    // name and location of object we want to find closest other to.
    const Point m_location;
    const string m_name;
};

// Helper template function for finding closest objects to another
// when used to search a map container of shared_ptrs to Sim_objects function will
// return a shared_ptr to the closest object to obj_ptr
template <typename C>
typename C::mapped_type Model::get_closest_helper(C& container, std::shared_ptr<Sim_object> obj_ptr) {
    // Get and iterator to the closest object to obj_ptr
    typename C::iterator iter = std::min_element(container.begin(), container.end(), Closest_to_obj(obj_ptr));

    // Check if a valid min element was found, return empty ptr if none found
    if (iter == container.end() || iter->second->get_name() == obj_ptr->get_name()) {
        return typename C::mapped_type();
    }

    return iter->second;
}

shared_ptr<Structure> Model::get_closest_structure_to_obj(shared_ptr<Sim_object> obj_ptr) {
    return get_closest_helper(m_structures, obj_ptr);
}

bool Model::is_agent_present(const string& name) const {
    auto iter = m_agents.find(name);

    // return false if agent with name not found, return true otherwise
    return iter != m_agents.end();
}

// add a new agent; assumes none with the same name
void Model::add_agent(shared_ptr<Agent> new_agent_ptr) {
    // Add Agent to Sim_objects container as a shared_ptr<Sim_object>
    m_sim_objs[new_agent_ptr->get_name()] = static_pointer_cast<Sim_object>(new_agent_ptr);
    // Add Agent to Agents container as an shared_ptr<Agent>
    m_agents[new_agent_ptr->get_name()] = new_agent_ptr;

    new_agent_ptr->broadcast_current_state();
}

// Remove Agent from Sim_objects container and Agents container
// Agent should be present in both containers when remove_agent
// is called
void Model::remove_agent(shared_ptr<Agent> agent_ptr) {
    assert(agent_ptr); // assert obj_ptr not nullptr

    auto agent_iter = m_agents.find(agent_ptr->get_name());
    assert(agent_iter != m_agents.end());
    m_agents.erase(agent_iter);

    auto obj_iter = m_sim_objs.find(agent_ptr->get_name());
    assert(obj_iter != m_sim_objs.end());
    m_sim_objs.erase(obj_iter);
}

shared_ptr<Agent> Model::get_agent_ptr(const string& name) const {
    // TODO this is code copy I think
    auto iter = m_agents.find(name);

    // throw Error if Agent not found
    if (iter == m_agents.end()) {
        throw Error("Agent not found!");
    }

    return iter->second;
}

shared_ptr<Agent> Model::get_closest_agent_to_obj(shared_ptr<Sim_object> obj_ptr) {
    return get_closest_helper(m_agents, obj_ptr);
}

static bool operator==(shared_ptr<Group> ptr, const string& name) {
    return *ptr == name;
}

// is there a group with this name?
bool Model::is_group_present(const std::string& name) const {
    auto iter = find(m_groups.begin(), m_groups.end(), name);

    // if group is return true, return false otherwise
    return iter != m_groups.end();
}

// Add new group, assumes none with same name already exists
void Model::add_group(std::shared_ptr<Group> group_ptr) {
    m_groups.push_back(group_ptr);
}

// Remove a Group with matching name, throws and Error if no Group with
// that name exists
void Model::remove_group(const string& name) {
    // Try to locate the Group with name in container
    auto iter = find(m_groups.begin(), m_groups.end(), name);

    // If group not found throw and Error
    if (iter == m_groups.end()) {
        throw Error("Group not found!");
    }

    // Group was found, remove it from the container
    m_groups.erase(iter);
}

shared_ptr<Group> Model::get_group_ptr(const string& name) const {
    auto iter = find(m_groups.begin(), m_groups.end(), name);

    // If group not found throw and Error
    if (iter == m_groups.end()) {
        throw Error("Group not found!");
    }

    return *iter;
}

// tell all objects to describe themselves to the console
void Model::describe() const {
    for_each(m_sim_objs.begin(), m_sim_objs.end(),
        [](const Sim_objs_t::value_type& p){ p.second->describe(); });

    for_each(m_groups.begin(), m_groups.end(),
        [](const shared_ptr<Group>& p){ p->describe(); });
}

// increment the time, and tell all objects to update themselves
void Model::update() {
    m_time++;

    for_each(m_sim_objs.begin(), m_sim_objs.end(),
        [](Sim_objs_t::value_type& p){ p.second->update(); });
}

/* View services */

// Attaching a View adds it to the container and causes it to be updated
// with all current objects'location (or other state information.
void Model::attach(shared_ptr<View> view_ptr) {
    m_views.push_back(view_ptr);

    for_each(m_sim_objs.begin(), m_sim_objs.end(),
        [](Sim_objs_t::value_type& p){ p.second->broadcast_current_state(); });
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(shared_ptr<View> view_ptr) {
    auto iter = find(m_views.begin(), m_views.end(), view_ptr);
    assert(iter != m_views.end());
    m_views.erase(iter);
}

// notify the views about an object's location
void Model::notify_location(const string& name, const Point& location) {
    for_each(m_views.begin(), m_views.end(),
        [&name, &location](shared_ptr<View>& v){ v->update_location(name, location); });
}

// notify the views that an object is now gone
void Model::notify_gone(const string& name) {
    for_each(m_views.begin(), m_views.end(),
        [&name](shared_ptr<View>& v){ v->update_remove(name); });
}

// notify the views of an objects's health
void Model::notify_health(const string& name, double health) {
    for_each(m_views.begin(), m_views.end(),
        [&name, health](shared_ptr<View>& v){ v->update_health(name, health); });
}

// notify the views of an object's food amount
void Model::notify_amount(const string& name, double amount) {
    for_each(m_views.begin(), m_views.end(),
        [&name, amount](shared_ptr<View>& v){ v->update_amount(name, amount); });
}

// TODO remove this
// notify the views to draw their information
void Model::notify_draw() {
    for (shared_ptr<View> p : m_views) {
        p->draw();
    }
}

// Returns a pointer to the View that matches name, returns an empty
// pointer otherwise
shared_ptr<View> Model::find_view(const string& name) {
    auto iter = m_views.begin();
    for (; iter != m_views.end(); iter++) {
        if ((*iter)->get_name() == name) {
            break;
        }
    }

    if (iter == m_views.end()) {
        return shared_ptr<View>();
    }

    return *iter;
}
