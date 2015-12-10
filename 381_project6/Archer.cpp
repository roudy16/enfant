#include "Archer.h"
#include "Structure.h"
#include "Model.h"
#include <string>
#include <iostream>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::static_pointer_cast;


// Initial attribute values for Archer class
constexpr int kARCHER_INITIAL_HEALTH = 6;
constexpr int kARCHER_INITIAL_STRENGTH = 2;
constexpr double kARCHER_INITIAL_RANGE = 6.0;


Archer::Archer(const string& name_, Point location_)
    : Infantry(name_, location_, kARCHER_INITIAL_HEALTH)
{
}

void Archer::do_update() {
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
