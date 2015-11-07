#include "Agent.h"
#include "Model.h"
#include "Utility.h"
#include <iostream>

using namespace std;

const double kAGENT_INITIAL_SPEED = 5.0;
const int kAGENT_INITIAL_HEALTH = 5;

Agent::Agent(const std::string& name_, Point location_)
    : Sim_object(name_), Moving_object(location_, kAGENT_INITIAL_SPEED),
    m_health(kAGENT_INITIAL_HEALTH), m_alive_state(Alive_State::ALIVE)
{
    cout << "Agent " << get_name() << " constructed" << endl;
}

Agent::~Agent() {
    cout << "Agent " << get_name() << " destructed" << endl;
}

Point Agent::get_location() const {
    return get_current_location();
}

bool Agent::is_moving() const {
    return is_currently_moving();
}

void Agent::move_to(Point destination_) {
    start_moving(destination_);

    if (is_currently_moving()) {
        cout << get_name() << ": I'm on the way" << endl;
    }
    else {
        cout << get_name() << ": I'm already there" << endl;
    }
}

void Agent::stop() {
    // if already stopped, do nothing
    if (!is_currently_moving()) {
        return;
    }

    // otherwise, stop moving and print message
    stop_moving();
    cout << get_name() << ": I'm stopped" << endl;
}

void Agent::take_hit(int attack_strength, Agent *attacker_ptr) {
    lose_health(attack_strength);
}

// update the moving state and Agent state of this object.
void Agent::update() {
    switch (m_alive_state) {
    case Alive_State::ALIVE:
        if (is_currently_moving()) {
            // update position and have Model notify View
            bool has_arrived = update_location();
            g_Model_ptr->notify_location(get_name(), get_current_location());

            if (has_arrived) {
                cout << get_name() << ": I'm there!" << endl;
            }
            else {
                cout << get_name() << ": step..." << endl;
            }
        }
        break;
    case Alive_State::DYING:
        m_alive_state = Alive_State::DEAD;
        // Tell Model to notify View to remove agent
        g_Model_ptr->notify_gone(get_name());
        break;
    case Alive_State::DEAD:
        m_alive_state = Alive_State::DISAPPEARING;
        break;
    case Alive_State::DISAPPEARING:
        // Do nothing
        break;
    default:
        throw Error("Unrecognized state in Agent::update");
    }
}

// output information about the current state
void Agent::describe() const {
    cout << get_name() << " at " << get_current_location() << endl;

    switch (m_alive_state) {
    case Alive_State::ALIVE:
        cout << "   Health is " << m_health << endl;
        if (is_currently_moving()) {
            cout << "   Moving at speed " << get_current_speed() 
                 << " to " << get_current_destination() << endl;
        }
        else {
            cout << "   Stopped" << endl;
        }
        break;
    case Alive_State::DYING:
        cout << "   Is dying" << endl;
        break;
    case Alive_State::DEAD:
        cout << "   Is dead" << endl;
        break;
    case Alive_State::DISAPPEARING:
        cout << "   Is disappearing" << endl; // not expected to be visible in this project
        break;
    default:
        throw Error("Unrecognized state in Agent::describe");
    }
}

// ask Model to broadcast our current state to all Views
void Agent::broadcast_current_state() const {
    // remove agents that aren't ALIVE
    if (m_alive_state != Alive_State::ALIVE) {
        g_Model_ptr->notify_gone(get_name());
        return;
    }

    // Tell View where ALIVE agents are
    g_Model_ptr->notify_location(get_name(), get_current_location());
}

/* Fat Interface for derived classes */
// Throws exception that an Agent cannot work.
void Agent::start_working(Structure *, Structure *) {
    throw Error(get_name() + ": Sorry, I can't work!");
}

// Throws exception that an Agent cannot attack.
void Agent::start_attacking(Agent *) {
    throw Error(get_name() + ": Sorry, I can't attack!");
}

// calculate loss of health due to hit.
// if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
void Agent::lose_health(int attack_strength) {
    m_health -= attack_strength;

    // Check if agent received fatal wound
    if (m_health <= 0) {
        m_alive_state = Alive_State::DYING;
        stop_moving();
        cout << get_name() << ": Arrggh!" << endl;
    }
    else {
        cout << get_name() << ": Ouch!" << endl;
    }
}
