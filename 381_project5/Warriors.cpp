#include "Warriors.h"
#include "Utility.h"
#include "Geometry.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

constexpr int kSOLDIER_INITIAL_STRENGTH = 2;
constexpr double kSOLDIER_INITIAL_RANGE = 2.0;
constexpr int kARCHER_INITIAL_STRENGTH = 1;
constexpr double kARCHER_INITIAL_RANGE = 6.0;


Infantry::Infantry(const string& name, Point location)
    : Agent(name, location), m_infantry_state(Infantry_state::NOT_ATTACKING)
{
    cout << "Infantry " << get_name() << " constructed" << endl;
}

Infantry::~Infantry()
{
}

void Infantry::stop_attacking() {
    m_infantry_state = Infantry_state::NOT_ATTACKING;
}

void Infantry::engage_new_target(shared_ptr<Agent> new_target) {
    m_target = static_cast<weak_ptr<Agent>>(new_target);
    cout << get_name() << ": I'm attacking!" << endl;
    m_infantry_state = Infantry_state::ATTACKING;
}

// output information about the current state
void Infantry::describe() const {
    cout << get_type_string() + ' ';
    Agent::describe();

    switch (m_infantry_state) {
    case Infantry_state::ATTACKING:
        if (m_target.expired()) {
            cout << "   Attacking dead target" << endl;
        }
        else {
            cout << "   Attacking " << m_target.lock()->get_name() << endl;
        }
        break;
    case Infantry_state::NOT_ATTACKING:
        cout << "   Not attacking" << endl;
        break;
    default:
        throw Error("Unrecognized state in Infantry::describe");
    }
}

// Overrides Agent's stop to print a message
void Infantry::stop() {
    cout << get_name() << ": Don't bother me" << endl;
}

// Make this Infantry start attacking the target Agent.
// Throws an exception if the target is the same as this Agent,
// is out of range, or is not alive.
void Infantry::start_attacking(shared_ptr<Agent>& target_ptr) {
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
    if (cartesian_distance(get_location(), target_ptr->get_location()) > get_range()) {
        throw Error(get_name() + ": Target is out of range!");
    }

    // At this point it is okay to attack the target
    engage_new_target(target_ptr);
}

std::weak_ptr<Agent>& Infantry::get_target() {
    return m_target;
}

Infantry::Infantry_state Infantry::get_state() const {
    return m_infantry_state;
}

Soldier::Soldier(const string& name_, Point location_)
    : Infantry(name_, location_)
{
    cout << "Soldier " << get_name() << " constructed" << endl;
}

Soldier::~Soldier() {
    cout << "Soldier " << get_name() << " destructed" << endl;
}

void Soldier::update() {
    Agent::update();

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
    if (cartesian_distance(get_location(), get_target().lock()->get_location()) > get_range()) {
        cout << get_name() << ": Target is now out of range" << endl;
        stop_attacking();
        return;
    }

    // target is in range, aim to maim!
    cout << get_name() << ": Clang!" << endl;
    shared_ptr<Agent> this_ptr = static_pointer_cast<Agent>(shared_from_this());
    get_target().lock()->take_hit(get_strength(), this_ptr);

    // If Infantry killed the target, report it, stop attacking and forget target
    if (get_target().expired()) {
        cout << get_name() << ": I triumph!" << endl;
        stop_attacking();
    }
}
// Overrides Agent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, shared_ptr<Agent>& attacker_ptr) {
    Agent::lose_health(attack_strength);

    // check if Soldier was just killed and was attacking
    if (get_state() == Infantry_state::ATTACKING) {
        // Soldier is dead, rest in peace warrior
        stop_attacking();
    } 
    // Attack the attacker if still alive and not already engaged
    else if (is_alive() && attacker_ptr->is_alive() &&
             get_state() == Infantry_state::NOT_ATTACKING)
    {
        engage_new_target(attacker_ptr);
    }
}

double Soldier::get_range() const {
    return kSOLDIER_INITIAL_RANGE;
}

int Soldier::get_strength() const {
    return kSOLDIER_INITIAL_STRENGTH;
}

const string& Soldier::get_type_string() const {
    static const string my_type = "Soldier";
    return my_type;
}

Archer::Archer(const string& name_, Point location_)
    : Infantry(name_, location_)
{
}

Archer::~Archer()
{
}

void Archer::update() {
    Agent::update();


}

void Archer::stop_attacking() {

}

const string& Archer::get_type_string() const {
    static const string my_type = "Archer";
    return my_type;
}
