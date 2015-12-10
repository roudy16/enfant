#ifndef INFANTRY_H
#define INFANTRY_H

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

    Infantry(const std::string& name_, Point location_, int start_health_);

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
#endif // INFANTRY_H
