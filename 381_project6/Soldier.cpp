#include "Soldier.h"
#include <string>
#include <iostream>
#include <memory>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::static_pointer_cast;

// Initial attribute values for Soldier class
constexpr int kSOLDIER_INITIAL_HEALTH = 10;
constexpr int kSOLDIER_INITIAL_STRENGTH = 3;
constexpr double kSOLDIER_INITIAL_RANGE = 2.0;


Soldier::Soldier(const string& name_, Point location_)
    : Infantry(name_, location_, kSOLDIER_INITIAL_HEALTH)
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
    // otherwise attack target
    if (target_in_range()) {
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
