#include "Infantry.h"
#include "Utility.h"
#include "Structure.h"
#include "Agent.h"
#include "Model.h"
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::weak_ptr; using std::static_pointer_cast; using std::make_shared;


Infantry::Infantry(const string& name, const Point& location, int start_health_)
    : Agent(name, location, start_health_), m_infantry_state(Infantry_state::NOT_ATTACKING)
{
}

void Infantry::stop_attacking() {
    m_infantry_state = Infantry_state::NOT_ATTACKING;
}

// Have Infantry set a new target and attack it!
void Infantry::engage_new_target(shared_ptr<Agent> new_target) {
    mp_target = weak_ptr<Agent>(new_target);
    cout << get_name() << ": I'm attacking!" << endl;
    m_infantry_state = Infantry_state::ATTACKING;
}

// output information about the current state
void Infantry::describe() const {
    cout << get_type_string() + ' ';
    Agent::describe();

    switch (m_infantry_state) {
    case Infantry_state::ATTACKING:
        if (!is_target_alive()) {
            cout << "   Attacking dead target" << endl;
        }
        else {
            cout << "   Attacking " << mp_target.lock()->get_name() << endl;
        }
        break;
    case Infantry_state::NOT_ATTACKING:
        cout << "   Not attacking" << endl;
        break;
    default:
        throw Error("Unrecognized state in Infantry::describe");
    }
}

void Infantry::do_update() {
    // do nothing by default
}

void Infantry::update() {
    assert(is_alive()); // should never update dead Agent

    // All Agent derived classes call Agent::update() before performing
    // specialized behaviors.
    Agent::update();
    do_update();
}

// Overrides Agent's stop to print a message
void Infantry::stop() {
    cout << get_name() << ": Don't bother me" << endl;
}

// Returns true if target is within attack range, prints message and stops
// attacking otherwise. Requires mp_target points to valid target
bool Infantry::target_in_range() {
    assert(is_target_alive());

    if (cartesian_distance(get_location(), mp_target.lock()->get_location()) > get_range())
    {
        // if target is out of range, report it, stop attacking and forget target
        cout << get_name() << ": Target is now out of range" << endl;
        stop_attacking();
        return false;
    }

    return true;
}

// Returns true if the target is alive, false otherwise
bool Infantry::is_target_alive() const {
    return !mp_target.expired() && mp_target.lock()->is_alive();
}

// Make this Infantry start attacking the target Agent.
// Throws an exception if the target is the same as this Agent,
// is out of range, or is not alive.
void Infantry::start_attacking(shared_ptr<Agent> target_ptr) {
    assert(target_ptr);

    // Ensure infantry does not attack self
    if (target_ptr == shared_from_this()) {
        throw Error(get_name() + ": I cannot attack myself!");
    }

    // Check target is Alive, cannot attack target if not Alive
    if (!target_ptr->is_alive()) {
        throw Error(get_name() + ": Target is not alive!");
    }

    // Check if target is in range, cannot attack out of range target
    const double distance = cartesian_distance(get_location(), target_ptr->get_location());
    if (distance > get_range()) {
        throw Error(get_name() + ": Target is out of range!");
    }

    // At this point it is okay to attack the target
    engage_new_target(target_ptr);
}

const weak_ptr<Agent>& Infantry::get_target() const noexcept {
    return mp_target;
}

Infantry::Infantry_state Infantry::get_state() const noexcept {
    return m_infantry_state;
}

// Comparator for finding closest object to another object. The Comparator is 
// initialized with the unique name of an object that we want to find the closest 
// other object to. When used to search a container of objects, object that evaluates
// as least is the object that is closest to the object used to init. In the event
// of a tie the object with the lesser lexicographical name is least
class Closest_to_obj {
public:
    Closest_to_obj(std::shared_ptr<Sim_object> obj_ptr)
        : m_location(obj_ptr->get_location()), m_name(obj_ptr->get_name())
    {}

    template <typename T>
    bool operator()(T& lhs, T& rhs) const {
        return closest_comp_helper(lhs.second, rhs.second);
    }

protected:
    // Compare distances and names of objects
    // Note: this template only works with std::map iterators that point
    // to Sim_objects or objects derived some Sim_object
    bool closest_comp_helper(std::shared_ptr<Sim_object> lhs,
        std::shared_ptr<Sim_object> rhs) const
    {
        // Check if either argument is the object used to init
        // Object used to init always evaluates greater than any other
        if (lhs->get_name() == m_name) {
            return false;
        }
        if (rhs->get_name() == m_name) {
            return true;
        }

        // get distances from args to init object
        double dist_to_lhs = cartesian_distance(m_location, lhs->get_location());
        double dist_to_rhs = cartesian_distance(m_location, rhs->get_location());

        // resolves distance ties with name comparison
        if (dist_to_lhs == dist_to_rhs) {
            return lhs->get_name() < rhs->get_name();
        }

        // return true if lhs is closer than rhs
        return dist_to_lhs < dist_to_rhs;
    }

private:
    // name and location of object we want to find closest other to.
    const Point m_location;
    const std::string m_name;
};

// Comparator used to find closest Agent that is not grouped with passed
// in agent
class Closest_hostile_to_agent : public Closest_to_obj {
public:
    Closest_hostile_to_agent(std::shared_ptr<Agent> agent)
        : Closest_to_obj(agent), m_agent(agent)
    {}

    bool operator()(Model::Agents_t::value_type& lhs,
                    Model::Agents_t::value_type& rhs) const
    {
        if (m_agent->agents_share_group(lhs.second)) {
            return false;
        }

        return closest_comp_helper(lhs.second, rhs.second);
    }

private:
    std::shared_ptr<Agent> m_agent;
};

// Returns pointer to closest Structure to this Infantry if one exists,
// returns an empty pointer otherwise
shared_ptr<Structure> Infantry::get_closest_structure() {
    shared_ptr<Structure> structure = 
        Model::get_instance()->find_min_structure(Closest_to_obj(shared_from_this()));

    if (!structure || structure->get_name() == get_name()) {
        return shared_ptr<Structure>();
    }

    return structure;
}

// Returns pointer to closest non-grouped Agent to this Infantry if one exists,
// returns an empty pointer otherwise
shared_ptr<Agent> Infantry::get_closest_hostile() {
    shared_ptr<Agent> this_ptr = static_pointer_cast<Agent>(shared_from_this());
    // Get an iterator to the closest hostile agent
    shared_ptr<Agent> hostile_agent = 
        Model::get_instance()->find_min_agent(Closest_hostile_to_agent(this_ptr));

    // Check if a valid min element was found, return empty ptr if none found
    if (!hostile_agent || agents_share_group(hostile_agent)) {
        return shared_ptr<Agent>();
    }

    return hostile_agent;
}

