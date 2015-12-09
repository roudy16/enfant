#include "Model.h"
#include "Warriors.h"
#include "Structure.h"
#include "Utility.h"
#include "Geometry.h"
#include <iostream>
#include <string>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::weak_ptr; using std::static_pointer_cast; using std::make_shared;

// Strength and range values for Soldier and Archer class
constexpr int kSOLDIER_INITIAL_STRENGTH = 2;
constexpr double kSOLDIER_INITIAL_RANGE = 2.0;
constexpr int kARCHER_INITIAL_STRENGTH = 1;
constexpr double kARCHER_INITIAL_RANGE = 6.0;


Infantry::Infantry(const string& name, Point location)
    : Agent(name, location), m_infantry_state(Infantry_state::NOT_ATTACKING)
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
    Agent::update();
    do_update();
}

// Overrides Agent's stop to print a message
void Infantry::stop() {
    cout << get_name() << ": Don't bother me" << endl;
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

Soldier::Soldier(const string& name_, Point location_)
    : Infantry(name_, location_)
{
}

void Soldier::do_update() {
    // Do nothing else if Infantry is not alive or not attacking
    if (get_state() == Infantry_state::NOT_ATTACKING) {
        return;
    }

    // Infantry is attacking
    // if target is dead, report it, stop attacking and forget target
    if (get_target().expired()) {
        cout << get_name() << ": Target is dead" << endl;
        stop_attacking();
        return;
    }

    // target is still alive
    // if target is out of range, report it, stop attacking and forget target
    const double distance = cartesian_distance(get_location(), get_target().lock()->get_location());
    if (distance > kSOLDIER_INITIAL_RANGE) {
        cout << get_name() << ": Target is now out of range" << endl;
        stop_attacking();
        return;
    }

    // target is in range, aim to maim!
    cout << get_name() << ": Clang!" << endl;
    shared_ptr<Agent> this_ptr = static_pointer_cast<Agent>(shared_from_this());
    get_target().lock()->take_hit(kSOLDIER_INITIAL_STRENGTH, this_ptr);

    // If Infantry killed the target, report it, stop attacking and forget target
    if (get_target().expired()) {
        cout << get_name() << ": I triumph!" << endl;
        stop_attacking();
    }
}

// Overrides Agent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr) {
    Agent::lose_health(attack_strength);

    // Attack the attacker if still alive and not already engaged
    if (is_alive() && attacker_ptr->is_alive() &&
        get_state() == Infantry_state::NOT_ATTACKING)
    {
        engage_new_target(attacker_ptr);
    }
}

double Soldier::get_range() const {
    return kSOLDIER_INITIAL_RANGE;
}

const string& Soldier::get_type_string() const {
    static const string my_type = "Soldier";
    return my_type;
}

Archer::Archer(const string& name_, Point location_)
    : Infantry(name_, location_)
{
}

void Archer::do_update() {
    // Do nothing else if Archer is not alive or not attacking
    if (get_state() == Infantry_state::ATTACKING) {
        // Archer is attacking
        // if target is dead, report it, stop attacking and forget target
        if (get_target().expired()) {
            cout << get_name() << ": Target is dead" << endl;
            stop_attacking();
        }
        else if (cartesian_distance(get_location(), get_target().lock()->get_location())
                 > kARCHER_INITIAL_RANGE)
        {
            // if target is out of range, report it, stop attacking and forget target
            cout << get_name() << ": Target is now out of range" << endl;
            stop_attacking();
        }
        else {
            // target is in range, aim to maim!
            cout << get_name() << ": Twang!" << endl;
            shared_ptr<Agent> this_ptr = static_pointer_cast<Agent>(shared_from_this());
            get_target().lock()->take_hit(kARCHER_INITIAL_STRENGTH, this_ptr);

            // If Archer killed the target, report it, stop attacking and forget target
            if (get_target().expired()) {
                cout << get_name() << ": I triumph!" << endl;
                stop_attacking();
            }
        }
    }

    // If the Archer is not attacking at this point it will try to find another Agent
    // in range and attack it!
    if (get_state() == Infantry_state::NOT_ATTACKING) {
        // Ask Model for closest other Agent to this Archer
        auto this_ptr = static_pointer_cast<Sim_object>(shared_from_this());
        auto closest_agent = Model::get_instance()->get_closest_agent_to_obj(this_ptr);

        // Ensure the Model gave us a valid Agent ptr
        if (!closest_agent) {
            return;
        }

        // get distance to target
        const double dist_to_agent = cartesian_distance(get_location(), closest_agent->get_location());

        // do nothing if target is out of range
        if (dist_to_agent > get_range()) {
            return;
        }

        // target is in range, let loose!
        engage_new_target(closest_agent);
    }
}

void Archer::take_hit(int attack_strength, shared_ptr<Agent> attacker) {
    lose_health(attack_strength);

    // Do nothing else if we died
    if (!is_alive()) {
        return;
    }

    // Find nearest Structure to flee to if one exists
    auto this_ptr = static_pointer_cast<Sim_object>(shared_from_this());
    auto closest_structure = Model::get_instance()->get_closest_structure_to_obj(this_ptr);

    // Do nothing if no structures exist
    if (!closest_structure) {
        return;
    }

    // Run away!
    cout << get_name() << ": I'm going to run away to " << closest_structure->get_name() << endl;
    move_to(closest_structure->get_location());
}

double Archer::get_range() const {
    return kARCHER_INITIAL_RANGE;
}

const string& Archer::get_type_string() const {
    static const string my_type = "Archer";
    return my_type;
}
