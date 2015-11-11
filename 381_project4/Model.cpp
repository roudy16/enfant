#include "Model.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Utility.h"
#include "View.h"
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

Model* g_Model_ptr = nullptr;

Model::Model() : m_time(0) 
{
    g_Model_ptr = this;

    add_structure(create_structure("Rivendale", "Farm", Point(10., 10.)));
    add_structure(create_structure("Sunnybrook", "Farm", Point(0., 30.)));
    add_structure(create_structure("Shire", "Town_Hall", Point(20., 20.)));
    add_structure(create_structure("Paduca", "Town_Hall", Point(30., 30.)));

    add_agent(create_agent("Pippin", "Peasant", Point(5., 10.)));
    add_agent(create_agent("Merry", "Peasant", Point(0., 25.)));
    add_agent(create_agent("Zug", "Soldier", Point(20., 30.)));
    add_agent(create_agent("Bug", "Soldier", Point(15., 20.)));
}

Model::~Model() {
    // destroy all objects
    for (Sim_object* p : m_sim_objs) {
        delete p;
    }
}


// is name already in use for either agent or structure?
// return true if the name matches the name of an existing agent or structure
bool Model::is_name_in_use(const string& name) const {
    return is_structure_present(name) || is_agent_present(name);
}

bool Model::is_structure_present(const string& name) const {
    auto iter = m_structures.find(name);
    if (iter == m_structures.end()) {
        return false;
    }

    return true;
}

// add a new structure; assumes none with the same name
void Model::add_structure(Structure* new_structure_ptr) {
    m_sim_objs.insert(static_cast<Sim_object*>(new_structure_ptr));
    m_structures[new_structure_ptr->get_name()] = new_structure_ptr;
    new_structure_ptr->broadcast_current_state();
}

Structure* Model::get_structure_ptr(const string& name) const {
    auto iter = m_structures.find(name);

    // throw Error if structure not found
    if (iter == m_structures.end()) {
        throw Error("Structure not found!");
    }

    return iter->second;
}

bool Model::is_agent_present(const string& name) const {
    auto iter = m_agents.find(name);
    if (iter == m_agents.end()) {
        return false;
    }

    return true;
}

// add a new agent; assumes none with the same name
void Model::add_agent(Agent* new_agent_ptr) {
    m_sim_objs.insert(static_cast<Sim_object*>(new_agent_ptr));
    m_agents[new_agent_ptr->get_name()] = new_agent_ptr;
    new_agent_ptr->broadcast_current_state();
}

// will throw Error("Agent not found!") if no agent of that name
Agent* Model::get_agent_ptr(const string& name) const {
    auto iter = m_agents.find(name);

    // throw Error if structure not found
    if (iter == m_agents.end()) {
        throw Error("Agent not found!");
    }

    return iter->second;
}

// tell all objects to describe themselves to the console
void Model::describe() const {
    for (Sim_object* p : m_sim_objs) {
        p->describe();
    }
}

// increment the time, and tell all objects to update themselves
void Model::update() {
    m_time++;

    // update all objects
    for (Sim_object* p : m_sim_objs) {
        p->update();
    }

    // collect disappearing agents
    vector<Agent*> disappearing_agents;
    for (auto& pair : m_agents) {
        if (pair.second->is_disappearing()) {
            disappearing_agents.push_back(pair.second);
        }
    }

    // remove disappearing agents from simulation
    for (Agent* agent_p : disappearing_agents) {
        const string& agent_name = agent_p->get_name();

        // remove agent from Model containers
        m_agents.erase(agent_name);
        auto sim_obj_iter = m_sim_objs.find(static_cast<Sim_object*>(agent_p));
        assert(sim_obj_iter != m_sim_objs.end());
        m_sim_objs.erase(sim_obj_iter);

        delete agent_p;
    }
}

/* View services */

// Attaching a View adds it to the container and causes it to be updated
// with all current objects'location (or other state information.
void Model::attach(View* view_ptr) {
    m_views.push_back(view_ptr);

    for (Sim_object* p : m_sim_objs) {
        view_ptr->update_location(p->get_name(), p->get_location());
    }
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(View* view_ptr) {
    auto iter = find(m_views.begin(), m_views.end(), view_ptr);
    assert(iter != m_views.end());
    m_views.erase(iter);
}

// notify the views about an object's location
void Model::notify_location(const std::string& name, Point location) {
    for (View* p : m_views) {
        p->update_location(name, location);
    }
}

// notify the views that an object is now gone
void Model::notify_gone(const std::string& name) {
    for (View* p : m_views) {
        p->update_remove(name);
    }
}
