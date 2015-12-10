#include "Mage.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::static_pointer_cast;


// Initial attribute values for Mage class
constexpr int kMAGE_INITIAL_HEALTH = 4;
constexpr int kMAGE_INITIAL_STRENGTH = 5;
constexpr double kMAGE_INITIAL_RANGE = 6.0;
constexpr int kMAGE_INITIAL_CHARGES = 2;
// Maximum charges a Mage can have and turns to regain a charge
constexpr int kMAGE_MAX_CHARGES = 2;
constexpr int kMAGE_RECHARGE_TIME = 3;


Mage::Mage(const string& name_, const Point& location_)
    : Infantry(name_, location_, kMAGE_INITIAL_HEALTH),
      m_charges(kMAGE_INITIAL_CHARGES), m_recharge_cooldown_timer(kMAGE_RECHARGE_TIME)
{
}

void Mage::teleport(Cartesian_vector dir) {
    // assert that direction is normalised
    assert(double_tolerance_compare_eq(dir.delta_x * dir.delta_x + dir.delta_y * dir.delta_y, 1.0));

    // TODO this right?
    if (m_charges == 0) {
        cout << get_name() << ": Out of charges" << endl;
    }

    // Expend a charge for teleportation
    --m_charges;

    // Calculate teleport target Point
    const Point& current_pos = get_location();
    Point target(current_pos.x + kMAGE_INITIAL_RANGE * dir.delta_x,
                 current_pos.y + kMAGE_INITIAL_RANGE * dir.delta_y);

    Agent::jump_to_location(target);
    cout << get_name() << ": Poof! I'm over here!" << endl;
}

// If the Mage has charges it will use them to teleport away from its attacker
// before the hit can strike the Mage, otherwise the Mage takes damage flees
// directly away from the attacker. If the attacker is at the same Point as the
// Mage then the Mage will teleport/flee toward the nearest Structure
void Mage::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr) {
    if (m_charges > 0) {

    }

}

// do update tasks for Mage
void Mage::do_update() {
    // Ensure member variables stay within expected range, should catch logic
    // errors related to use of these variables.
    assert(m_charges >= 0 && m_charges <= kMAGE_MAX_CHARGES);
    assert(m_recharge_cooldown_timer >= 0 && m_recharge_cooldown_timer <= kMAGE_RECHARGE_TIME);

    // If Mage is not at max charges check to see if enough time has passed to
    // regain a charge.
    if (m_charges != kMAGE_MAX_CHARGES) {
        --m_recharge_cooldown_timer;
        if (m_recharge_cooldown_timer == 0) {
            ++m_charges;
            m_recharge_cooldown_timer = kMAGE_RECHARGE_TIME;
        }
    }

    if (get_state() == Infantry_state::ATTACKING) {
        // Mage is attacking
        // if target is dead, report it, stop attacking and forget target
        if (get_target().expired()) {
            cout << get_name() << ": Target is dead" << endl;
            stop_attacking();
        }
        else if (target_in_range())
        {
            // Check if Mage has charges to use for attack
            if (m_charges == 0) {
                cout << get_name() << ": Must recharge before I attack..." << endl;
            }
            else {
                // target is in range, aim to maim!
                // Use of attack spell expends a charge.
                cout << get_name() << ": FWOOoosh!" << endl;
                shared_ptr<Agent> this_ptr = static_pointer_cast<Agent>(shared_from_this());
                get_target().lock()->take_hit(kMAGE_INITIAL_STRENGTH, this_ptr);

                // If Mage killed the target, report it, stop attacking and forget target
                if (get_target().expired()) {
                    cout << get_name() << ": Play with fire, you get burned!" << endl;
                    stop_attacking();
                }
            }
        }
    } // End ATTACKING state logic
}

// returns Mage's attack range
double Mage::get_range() const {
    return kMAGE_INITIAL_RANGE;
}

// return string "Mage"
const string& Mage::get_type_string() const {
    static const string my_type = "Mage";
    return my_type;
}
