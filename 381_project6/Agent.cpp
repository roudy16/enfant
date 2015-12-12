#include "Agent.h"
#include "Model.h"
#include "Utility.h"
#include "Death_observer.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr; using std::static_pointer_cast;
using std::for_each; using std::find;

const double kAGENT_INITIAL_SPEED = 5.0;

Agent::Agent(const string& name_, const Point& location_, int start_health_)
    : Sim_object(name_), m_moving_obj(location_, kAGENT_INITIAL_SPEED),
    m_health(start_health_), m_alive_state(Alive_State::ALIVE)
{
}

Agent::~Agent()
{
}

Point Agent::get_location() const {
    return m_moving_obj.get_current_location();
}

bool Agent::is_moving() const {
    return m_moving_obj.is_currently_moving();
}

void Agent::move_to(const Point& destination_) {
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

// calculate loss of health due to hit.
// if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
void Agent::lose_health(int attack_strength) {
    // Apply damage to Agent's health
    m_health -= attack_strength;

    // Check if agent received fatal wound
    if (m_health <= 0) {
        // Agent was killed, set Agent to dead state
        m_alive_state = Alive_State::DEAD;
        m_moving_obj.stop_moving();
        cout << get_name() << ": Arrggh!" << endl;

        // notify Model to remove Agent from simulation
        auto this_ptr = static_pointer_cast<Agent>(shared_from_this());
        notify_death();
        Model::get_instance()->notify_gone(get_name());
        Model::get_instance()->remove_agent(this_ptr);
    }
    else {
        // Acknowledge damage and notify of Model of updated health
        cout << get_name() << ": Ouch!" << endl;
        Model::get_instance()->notify_health(get_name(), static_cast<double>(m_health));
    }
}

void Agent::take_hit(int attack_strength, shared_ptr<Agent> attacker_ptr) {
    lose_health(attack_strength);
}

// update the moving state and Agent state of this object.
void Agent::update() {
    // Should never update dead Agent
    assert(m_alive_state == Alive_State::ALIVE);

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
        cout << "   Is dead" << endl; // not expected to be visible in this project
        break;
    default:
        throw Error("Unrecognized state in Agent::describe");
    }
}

// ask Model to broadcast our current state to all Views
void Agent::broadcast_current_state() const {
    assert(m_alive_state == Alive_State::ALIVE);
    // Tell Views where agents are and their health
    Model::get_instance()->notify_location(get_name(), get_location());
    Model::get_instance()->notify_health(get_name(), static_cast<double>(m_health));
}

/* Fat Interface for derived classes */
// Throws exception that an Agent cannot work.
void Agent::start_working(shared_ptr<Structure> dst, shared_ptr<Structure> src) {
    throw Error(get_name() + ": Sorry, I can't work!");
}

// Throws exception that an Agent cannot attack.
void Agent::start_attacking(shared_ptr<Agent> target) {
    throw Error(get_name() + ": Sorry, I can't attack!");
}

// Jump Agent to target location
void Agent::jump_to_location(const Point& target) {
    m_moving_obj.jump_to_location(target);
    Model::get_instance()->notify_location(get_name(), get_location());
}

void Agent::attach_death_observer(std::shared_ptr<Death_observer> observer) {
    assert(find(m_death_observers.begin(), m_death_observers.end(), observer) == m_death_observers.end());

    m_death_observers.push_back(observer);
}

void Agent::detach_death_observer(std::shared_ptr<Death_observer> observer) {
    auto iter = find(m_death_observers.begin(), m_death_observers.end(), observer);

    // function should not be called if observer is not in this Agents observer container
    assert(iter != m_death_observers.end());

    m_death_observers.erase(iter);
}

void Agent::notify_death() {
    const string& name = get_name();
    for_each(m_death_observers.begin(), m_death_observers.end(),
        [&name](shared_ptr<Death_observer>& p){ p->update_on_death(name); });
}


