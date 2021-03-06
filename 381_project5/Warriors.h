#ifndef WARRIORS_H
#define WARRIORS_H

#include "Agent.h"
#include <string>
#include <memory>

// Infantry is an abstract base class that represents the basic capabilities
// of combatants. They can report about themselves and attack other Agents
class Infantry : public Agent {
public:
    // output information about the current state
    void describe() const override;

    // Does Agent class update as well as do_update hook that base classes
    // can provide
    void update() override final;

    // Overrides Agent's stop to print a message
    void stop() override;

    // Make this Infantry start attacking the target Agent.
    // Throws an exception if the target is the same as this Agent,
    // is out of range, or is not alive.
    void start_attacking(std::shared_ptr<Agent> target_ptr) override;

protected:
    enum class Infantry_state { NOT_ATTACKING, ATTACKING };

    Infantry(const std::string& name_, Point location_);

    // Accessors for derived classes to Infantry member variables
    const std::weak_ptr<Agent>& get_target();
    Infantry_state get_state() const;

    // stop attacking and forget target
    virtual void stop_attacking();

    // set new target and engage, outputs attacking message
    virtual void engage_new_target(std::shared_ptr<Agent> new_target);

    // update hook that is executed after Agent class update,
    // does nothing unless overridden
    virtual void do_update();

    // Accessor hooks derived classes must provide
    virtual const std::string& get_type_string() const = 0;
    virtual double get_range() const = 0;

private:
    std::weak_ptr<Agent> mp_target;
    Infantry_state m_infantry_state;
};

/*
A Soldier is an Agent that has attack and defense behaviors. It can be commanded
to start attacking another Agent and will continue the attack as long as 
it is alive and the target is alive and in range. If attacked, the Soldier will
start attacking its attacker.
*/

class Soldier : public Infantry {
public:
    explicit Soldier(const std::string& name_, Point location_);

    // Overrides Agent's take_hit to counterattack when attacked.
    void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr) override;

    // disallow copy/move construction or assignment and default ctor
    Soldier() = delete;
    Soldier(const Soldier&) = delete;
    Soldier& operator= (const Soldier&)  = delete;
    Soldier(Soldier&&) = delete;
    Soldier& operator= (Soldier&&) = delete;

private:
    // do update tasks for Soldier
    void do_update() override;
    // returns Soldier's attack range
    double get_range() const override;
    // returns string "Soldier"
    const std::string& get_type_string() const override;
};

class Archer : public Infantry {
public:
    explicit Archer(const std::string& name_, Point location_);

    // Overrides Agent's take_hit to counterattack when attacked.
    void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr) override;

    // disallow copy/move construction or assignment and default ctor
    Archer() = delete;
    Archer(const Archer&) = delete;
    Archer& operator= (const Archer&)  = delete;
    Archer(Archer&&) = delete;
    Archer& operator= (Archer&&) = delete;

private:
    // do update tasks for Archer
    void do_update() override;
    // returns Archer's attack range
    double get_range() const override;
    // return string "Archer"
    const std::string& get_type_string() const override;
};

#endif // WARRIORS_H
