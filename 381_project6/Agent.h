#ifndef AGENT_H
#define AGENT_H

#include <memory>
#include <vector>
#include "Moving_object.h"
#include "Sim_object.h"

/*
Agents are a kind of Sim_object, and have a Moving_object.
Agents can be commanded to move to a destination. Agents have a health, which
is decreased when they take a hit. If the Agent's health > 0, it is alive.
If its heath <= 0, it dies and tells Model to remove it.
*/


class Structure;
class Group;

class Agent : public Sim_object {
public:
    // Make Agent an abstract class
    virtual ~Agent() = 0;

    // return true if this agent is Alive or Disappearing
    bool is_alive() const { return m_alive_state == Alive_State::ALIVE; }

    // return true if this Agent is in motion
    bool is_moving() const;

    // return this Agent's location
    Point get_location() const override;

    // update the moving state and Agent state of this object.
    void update() override;

    // output information about the current state
    void describe() const override;

    // ask Model to broadcast our current state to all Views
    void broadcast_current_state() const override;

    // tell this Agent to start moving to location destination_
    virtual void move_to(const Point& destination_);

    // tell this Agent to stop its activity
    virtual void stop();

    // Tell this Agent to accept a hit from an attack of a specified strength
    // The attacking Agent identifies itself with its this pointer.
    // A derived class can override this function.
    // The function lose_health is called to handle the effect of the attack.
    virtual void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr);

    /* Fat Interface for derived classes */
    // Prints message that an Agent cannot work.
    virtual void start_working(std::shared_ptr<Structure>, std::shared_ptr<Structure>);

    // Prints message that an Agent cannot attack.
    virtual void start_attacking(std::shared_ptr<Agent>);

    // returns true if this Agent shares a group with the other Agent
    bool agents_share_group(std::shared_ptr<Agent> other_agent);

    // Allow Agents to keep pointers to the groups they are members of.
    void add_to_my_groups(std::shared_ptr<Group> group_ptr);
    void remove_from_my_groups(std::shared_ptr<Group> group_ptr);

protected:
    // Constructs an Agent with name_ at location_ with start_health_ health
    Agent(const std::string& name_, const Point& location_, int start_health_);

    // calculate loss of health due to hit.
    // if health decreases to zero or negative, Agent dies.
    void lose_health(int attack_strength);

    // Jump Agent to target location
    void jump_to_location(const Point& target);

private:
    enum class Alive_State { ALIVE, DEAD };

    std::vector<std::shared_ptr<Group>> m_groups;
    Moving_object m_moving_obj;
    int m_health;
    Alive_State m_alive_state;
};

#endif // AGENT_H
