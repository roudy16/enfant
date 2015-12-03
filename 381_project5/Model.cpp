#include "Model.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Structure.h"
#include "Agent.h"
#include "Utility.h"
#include "View.h"
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

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
    add_structure(create_structure("Rivendale", "Farm", Point(10., 10.)));
    add_structure(create_structure("Sunnybrook", "Farm", Point(0., 30.)));
    add_structure(create_structure("Shire", "Town_Hall", Point(20., 20.)));
    add_structure(create_structure("Paduca", "Town_Hall", Point(30., 30.)));

    add_agent(create_agent("Pippin", "Peasant", Point(5., 10.)));
    add_agent(create_agent("Merry", "Peasant", Point(0., 25.)));
    add_agent(create_agent("Zug", "Soldier", Point(20., 30.)));
    add_agent(create_agent("Bug", "Soldier", Point(15., 20.)));
}

// is name already in use for either agent or structure?
// return true if the name matches the name of an existing agent or structure
bool Model::is_name_in_use(const string& name) const {
    return is_structure_present(name) || is_agent_present(name);
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
    if (iter == m_structures.end()) {
        return false;
    }
    else {
        return true;
    }
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

bool Model::is_agent_present(const string& name) const {
    auto iter = m_agents.find(name);

    // return false if agent with name not found, return true otherwise
    if (iter == m_agents.end()) {
        return false;
    }
    else {
        return true;
    }
}

// add a new agent; assumes none with the same name
void Model::add_agent(shared_ptr<Agent> new_agent_ptr) {
    // Add Agent to Sim_objects container as a shared_ptr<Sim_object>
    m_sim_objs[new_agent_ptr->get_name()] = static_pointer_cast<Sim_object>(new_agent_ptr);

    // Add Agent to Structures container as an shared_ptr<Agent>
    m_agents[new_agent_ptr->get_name()] = new_agent_ptr;
    new_agent_ptr->broadcast_current_state();
}

void Model::remove_agent(std::shared_ptr<Agent> agent_ptr) {
    assert(agent_ptr); // assert agent_ptr not nullptr

    auto agent_iter = m_agents.find(agent_ptr->get_name());
    assert(agent_iter != m_agents.end());
    m_agents.erase(agent_iter);

    auto sim_obj_iter = m_sim_objs.find(agent_ptr->get_name());
    assert(sim_obj_iter != m_sim_objs.end());
    m_sim_objs.erase(sim_obj_iter);
}

shared_ptr<Agent> Model::get_agent_ptr(const string& name) const {
    auto iter = m_agents.find(name);

    // throw Error if structure not found
    if (iter == m_agents.end()) {
        throw Error("Agent not found!");
    }

    return iter->second;
}

shared_ptr<Agent> Model::get_closest_agent_to_obj(shared_ptr<Sim_object>&) {

}

// tell all objects to describe themselves to the console
void Model::describe() const {
    for (auto& pair : m_sim_objs) {
        pair.second->describe();
    }
}

// increment the time, and tell all objects to update themselves
void Model::update() {
    m_time++;

    // update all objects
    for (auto& pair : m_sim_objs) {
        pair.second->update();
    }
}

/* View services */

// Attaching a View adds it to the container and causes it to be updated
// with all current objects'location (or other state information.
void Model::attach(shared_ptr<View> view_ptr) {
    m_views.push_back(view_ptr);

    for (auto& p : m_sim_objs) {
        p.second->broadcast_current_state();
    }
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(shared_ptr<View>& view_ptr) {
    auto iter = find(m_views.begin(), m_views.end(), view_ptr);
    assert(iter != m_views.end());
    m_views.erase(iter);
}

// notify the views about an object's location
void Model::notify_location(const string& name, const Point& location) {
    for (shared_ptr<View>& p : m_views) {
        p->update_location(name, location);
    }
}

// notify the views that an object is now gone
void Model::notify_gone(const string& name) {
    for (shared_ptr<View>& p : m_views) {
        p->update_remove(name);
    }
}

// notify the views of an objects's health
void Model::notify_health(const string& name, double health) {
    for (shared_ptr<View>& p : m_views) {
        p->update_health(name, health);
    }
}

// notify the views of an object's food amount
void Model::notify_amount(const string& name, double amount) {
    for (shared_ptr<View>& p : m_views) {
        p->update_amount(name, amount);
    }
}

// notify the views to draw their information
void Model::notify_draw() {
    for (shared_ptr<View>& p : m_views) {
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
