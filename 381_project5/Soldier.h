#ifndef SOLDIER_H
#define SOLDIER_H

#include "Agent.h"
#include <string>
#include <memory>

/*
A Soldier is an Agent that has attack and defense behaviors. It can be commanded
to start attacking another Agent and will continue the attack as long as 
it is alive and the target is alive and in range. If attacked, the Soldier will
start attacking its attacker.
*/

class Soldier : public Agent {
public:
    Soldier(const std::string& name_, Point location_);

    ~Soldier();

    // update implements Soldier behavior
    void update() override;

    // Make this Soldier start attacking the target Agent.
    // Throws an exception if the target is the same as this Agent,
    // is out of range, or is not alive.
    void start_attacking(std::shared_ptr<Agent>& target_ptr) override;

    // Overrides Agent's take_hit to counterattack when attacked.
    void take_hit(int attack_strength, std::shared_ptr<Agent>& attacker_ptr) override;

    // Overrides Agent's stop to print a message
    void stop() override;

    // output information about the current state
    void describe() const override;

private:
    enum class Soldier_State { NOT_ATTACKING, ATTACKING };

    // stop attacking and forget target
    void stop_attacking();

    // set new target and engage, outputs attacking message
    void engage_new_target(std::shared_ptr<Agent> new_target);

    std::weak_ptr<Agent> m_target;
    double m_attack_range;
    int m_attack_strength;
    Soldier_State m_soldier_state;

    // disallow copy/move construction or assignment and default ctor
    Soldier() = delete;
    Soldier(const Soldier&) = delete;
    Soldier& operator= (const Soldier&)  = delete;
    Soldier(Soldier&&) = delete;
    Soldier& operator= (Soldier&&) = delete;
};

#endif // SOLDIER_H
