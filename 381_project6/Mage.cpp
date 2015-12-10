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
constexpr int kMAGE_MAX_CHARGES = 2;
constexpr int kMAGE_RECHARGE_TIME = 3; // number of turns to regain a charge


Mage::Mage(const string& name_, Point location_)
    : Infantry(name_, location_, kMAGE_INITIAL_HEALTH),
      m_charges(kMAGE_MAX_CHARGES), m_recharge_cooldown_timer(kMAGE_RECHARGE_TIME)
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

void Mage::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr) {

}

// do update tasks for Mage
void Mage::do_update() {

}

// returns Mage's attack range
double Mage::get_range() const {

}

// return string "Mage"
const string& Mage::get_type_string() const {
    static const string my_type = "Mage";
    return my_type;
}
