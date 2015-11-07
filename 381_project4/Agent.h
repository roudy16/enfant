#ifndef AGENT_H
#define AGENT_H

#include "Moving_object.h"
#include "Sim_object.h"

/*
Agents are a kind of Sim_object, and privately inherit from Moving_object.
Agents can be commanded to move to a destination. Agents have a health, which
is decreased when they take a hit. If the Agent's health > 0, it is alive.
If its heath <= 0, it starts dying, then on subsequent updates, 
it becomes dead, and finally disappearing.
*/

/* 
*** This skeleton file shows the required public interface for the class, which you may not modify. 
If no protected members are shown, there must be none in your version. 
If any protected or private members are shown here, then your class must also have them and use them as intended.
You must delete this comment and all other comments that start with "***".
*/


class Structure;

// *** declare as inheriting from Sim_object and Moving_object, as specified
class Agent : public Sim_object, private Moving_object {
public:
    ~Agent();

    // return true if this agent is Alive or Disappearing
    bool is_alive() const { return m_alive_state == Alive_State::ALIVE; }
    bool is_disappearing() const { return m_alive_state == Alive_State::DISAPPEARING; }

    // return this Agent's location
    Point get_location() const override;

    // return true if this Agent is in motion
    bool is_moving() const;

    // tell this Agent to start moving to location destination_
    virtual void move_to(Point destination_);

    // tell this Agent to stop its activity
    virtual void stop();

    // Tell this Agent to accept a hit from an attack of a specified strength
    // The attacking Agent identifies itself with its this pointer.
    // A derived class can override this function.
    // The function lose_health is called to handle the effect of the attack.
    virtual void take_hit(int attack_strength, Agent *attacker_ptr);

    // update the moving state and Agent state of this object.
    void update() override;

    // output information about the current state
    void describe() const override;

    // ask Model to broadcast our current state to all Views
    void broadcast_current_state() const override;

    /* Fat Interface for derived classes */
    // Throws exception that an Agent cannot work.
    virtual void start_working(Structure *, Structure *);

    // Throws exception that an Agent cannot attack.
    virtual void start_attacking(Agent *);

protected:
    Agent(const std::string& name_, Point location_);

    // calculate loss of health due to hit.
    // if health decreases to zero or negative, Agent state becomes Dying, and any movement is stopped.
    void lose_health(int attack_strength);

private:
    enum class Alive_State { ALIVE, DYING, DEAD, DISAPPEARING };

    int m_health;
    Alive_State m_alive_state;
};

#endif // AGENT_H
