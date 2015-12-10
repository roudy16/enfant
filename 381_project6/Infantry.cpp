#include "Infantry.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::weak_ptr; using std::static_pointer_cast; using std::make_shared;


Infantry::Infantry(const string& name, Point location, int start_health_)
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
        if (mp_target.expired()) {
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
// attacking otherwise.
bool Infantry::target_in_range() {
    assert(!mp_target.expired());
    if (cartesian_distance(get_location(), mp_target.lock()->get_location())
                           > get_range())
    {
        // if target is out of range, report it, stop attacking and forget target
        cout << get_name() << ": Target is now out of range" << endl;
        stop_attacking();
        return false;
    }

    return true;
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
    const double&& distance = cartesian_distance(get_location(), target_ptr->get_location());
    if (distance > get_range()) {
        throw Error(get_name() + ": Target is out of range!");
    }

    // At this point it is okay to attack the target
    engage_new_target(target_ptr);
}

const weak_ptr<Agent>& Infantry::get_target() {
    return mp_target;
}

Infantry::Infantry_state Infantry::get_state() const {
    return m_infantry_state;
}
