#include "Soldier.h"
#include "Utility.h"
#include "Geometry.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

const int kSOLDIER_INITIAL_STRENGTH = 2;
const double kSOLDIER_INITIAL_RANGE = 2.0;

Soldier::Soldier(const std::string& name_, Point location_)
    : Agent(name_, location_), m_target(nullptr), m_attack_range(kSOLDIER_INITIAL_RANGE),
    m_attack_strength(kSOLDIER_INITIAL_STRENGTH), m_soldier_state(Soldier_State::NOT_ATTACKING)
{
    cout << "Soldier " << get_name() << " constructed" << endl;
}

Soldier::~Soldier() {
    cout << "Soldier " << get_name() << " destructed" << endl;
}

void Soldier::stop_attacking() {
    m_soldier_state = Soldier_State::NOT_ATTACKING;
    m_target = nullptr;
}

void Soldier::update() {
    Agent::update();

    // Do nothing else if Soldier is not alive or not attacking
    if (!is_alive() || m_soldier_state == Soldier_State::NOT_ATTACKING) {
        assert(!m_target);
        return;
    }

    // Soldier is attacking
    assert(m_target);

    // if target is dead, report it, stop attacking and forget target
    if (!m_target->is_alive()) {
        cout << get_name() << ": Target is dead" << endl;
        stop_attacking();
        return;
    }

    // target is still alive
    // if target is out of range, report it, stop attacking and forget target
    if (cartesian_distance(get_location(), m_target->get_location()) > m_attack_range) {
        cout << get_name() << ": Target is now out of range" << endl;
        stop_attacking();
        return;
    }

    // target is in range, aim to maim!
    cout << get_name() << ": Clang!" << endl;
    m_target->take_hit(m_attack_strength, this);

    // If Soldier killed the target, report it, stop attacking and forget target
    if (!m_target->is_alive()) {
        cout << get_name() << ": I triumph!" << endl;
        stop_attacking();
    }
}

void Soldier::engage_new_target(Agent* new_target) {
    m_target = new_target;
    cout << get_name() << ": I'm attacking!" << endl;
    m_soldier_state = Soldier_State::ATTACKING;
}

// Make this Soldier start attacking the target Agent.
// Throws an exception if the target is the same as this Agent,
// is out of range, or is not alive.
void Soldier::start_attacking(Agent* target_ptr) {
    assert(target_ptr);

    // Ensure soldier does not attack self
    if (target_ptr == this) {
        throw Error(get_name() + ": I cannot attack myself!");
    }

    // Check target is Alive, cannot attack target if not Alive
    if (!target_ptr->is_alive()) {
        throw Error(get_name() + ": Target is not alive!");
    }

    // Check if target is in range, cannot attack out of range target
    if (cartesian_distance(get_location(), target_ptr->get_location()) > m_attack_range) {
        throw Error(get_name() + ": Target is out of range!");
    }

    // At this point it is okay to attack the target
    engage_new_target(target_ptr);
}

// Overrides Agent's take_hit to counterattack when attacked.
void Soldier::take_hit(int attack_strength, Agent* attacker_ptr) {
    Agent::lose_health(attack_strength);

    // check if Soldier was just killed and was attacking
    if (!is_alive() && m_soldier_state == Soldier_State::ATTACKING) {
        // Soldier is dead, rest in peace warrior
        stop_attacking();
    } 
    // Attack the attacker if still alive and not already engaged
    else if (is_alive() && attacker_ptr->is_alive() &&
             m_soldier_state == Soldier_State::NOT_ATTACKING)
    {
        engage_new_target(attacker_ptr);
    }
}

// Overrides Agent's stop to print a message
void Soldier::stop() {
    cout << get_name() << ": Don't bother me" << endl;
}

// output information about the current state
void Soldier::describe() const {
    cout << "Soldier ";
    Agent::describe();

    switch (m_soldier_state) {
    case Soldier_State::ATTACKING:
        assert(m_target);
        cout << "   Attacking " << m_target->get_name() << endl;
        break;
    case Soldier_State::NOT_ATTACKING:
        assert(!m_target);
        cout << "   Not attacking" << endl;
        break;
    default:
        throw Error("Unrecognized state in Soldier::describe");
    }
}
