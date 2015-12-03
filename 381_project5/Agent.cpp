#include "Agent.h"
#include "Model.h"
#include "Utility.h"
#include <iostream>
#include <cassert>

using namespace std;

const double kAGENT_INITIAL_SPEED = 5.0;
const int kAGENT_INITIAL_HEALTH = 5;

Agent::Agent(const std::string& name_, Point location_)
    : Sim_object(name_), m_moving_obj(location_, kAGENT_INITIAL_SPEED),
    m_health(kAGENT_INITIAL_HEALTH), m_alive_state(Alive_State::ALIVE)
{
    cout << "Agent " << get_name() << " constructed" << endl;
}

Agent::~Agent() {
    cout << "Agent " << get_name() << " destructed" << endl;
}

Point Agent::get_location() const {
    return m_moving_obj.get_current_location();
}

// return Agent's health
int Agent::get_health() const {
    return m_health;
}

bool Agent::is_moving() const {
    return m_moving_obj.is_currently_moving();
}

void Agent::move_to(Point destination_) {
    m_moving_obj.start_moving(destination_);

    if (m_moving_obj.is_currently_moving()) {
        cout << get_name() << ": I'm on the way" << endl;
    }
    else {
        cout << get_name() << ": I'm already there" << endl;
    }
}

void Agent::stop() {
    // if already stopped, do nothing
    if (!m_moving_obj.is_currently_moving()) {
        return;
    }

    // otherwise, stop moving and print message
    m_moving_obj.stop_moving();
    cout << get_name() << ": I'm stopped" << endl;
}

void Agent::take_hit(int attack_strength, shared_ptr<Agent> &attacker_ptr) {
    lose_health(attack_strength);
}

// update the moving state and Agent state of this object.
void Agent::update() {
    switch (m_alive_state) {
    case Alive_State::ALIVE:
        if (m_moving_obj.is_currently_moving()) {
            // update position and have Model notify View
            bool has_arrived = m_moving_obj.update_location();

            Model::get_instance()->notify_location(get_name(), get_location());

            if (has_arrived) {
                cout << get_name() << ": I'm there!" << endl;
            }
            else {
                cout << get_name() << ": step..." << endl;
            }
        }
        break;
    case Alive_State::DEAD:
        // Do nothing
    default:
        throw Error("Unrecognized state in Agent::update");
    }
}

// output information about the current state
void Agent::describe() const {
    cout << get_name() << " at " << m_moving_obj.get_current_location() << endl;

    switch (m_alive_state) {
    case Alive_State::ALIVE:
        cout << "   Health is " << m_health << endl;
        if (m_moving_obj.is_currently_moving()) {
            cout << "   Moving at speed " << m_moving_obj.get_current_speed() 
                 << " to " << m_moving_obj.get_current_destination() << endl;
        }
        else {
            cout << "   Stopped" << endl;
        }
        break;
    case Alive_State::DEAD:
        cout << "   Is dead" << endl;
        break;
    default:
        throw Error("Unrecognized state in Agent::describe");
    }
}

// ask Model to broadcast our current state to all Views
void Agent::broadcast_current_state() const {
    assert(m_alive_state == Alive_State::ALIVE);
    // Tell View where agents are
    Model::get_instance()->notify_location(get_name(), get_location());
    Model::get_instance()->notify_health(get_name(), static_cast<double>(m_health));
}

/* Fat Interface for derived classes */
// Throws exception that an Agent cannot work.
void Agent::start_working(shared_ptr<Structure>& dst, shared_ptr<Structure>& src) {
    throw Error(get_name() + ": Sorry, I can't work!");
}

// Throws exception that an Agent cannot attack.
void Agent::start_attacking(shared_ptr<Agent>& target) {
    throw Error(get_name() + ": Sorry, I can't attack!");
}

// calculate loss of health due to hit.
// if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
void Agent::lose_health(int attack_strength) {
    m_health -= attack_strength;

    // Check if agent received fatal wound
    if (m_health <= 0) {
        m_alive_state = Alive_State::DEAD;
        m_moving_obj.stop_moving();
        Model::get_instance()->notify_gone(get_name());
        cout << get_name() << ": Arrggh!" << endl;
        Model::get_instance()->notify_location(get_name(), get_location());
    }
    else {
        Model::get_instance()->notify_health(get_name(), static_cast<double>(m_health));
        cout << get_name() << ": Ouch!" << endl;
    }
}
