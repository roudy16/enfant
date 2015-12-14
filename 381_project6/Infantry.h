#ifndef INFANTRY_H
#define INFANTRY_H

#include "Agent.h"
#include <string>
#include <memory>

//TODO
#include "Utility.h"

// Forward declaration
struct Point;
class Structure;

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

    Infantry(const std::string& name_, const Point& location_, int start_health_);

    // Returns pointer to closest Structure to this Infantry if one exists,
    // returns an empty pointer otherwise
    std::shared_ptr<Structure> get_closest_structure();

    // Returns pointer to closest non-grouped Agent to this Infantry if one exists,
    // returns an empty pointer otherwise
    std::shared_ptr<Agent> get_closest_hostile();

    // Accessors for derived classes to Infantry member variables
    const std::weak_ptr<Agent>& get_target() const noexcept;
    Infantry_state get_state() const noexcept;

    // Returns true if target is within attack range, prints message and stops
    // attacking and returns false otherwise.
    bool target_range_handling();

    // Returns true if the target is alive, false otherwise
    bool is_target_alive() const;

    // stop attacking and forget target
    virtual void stop_attacking();

    // set new target and engage, outputs attacking message
    virtual void engage_new_target(std::shared_ptr<Agent> new_target);

    // update hook that is executed after Agent class update,
    // does nothing unless overridden
    virtual void do_update();

    // Accessor hooks derived classes must provide
    virtual const std::string& get_type_string() const noexcept = 0;
    virtual double get_range() const noexcept = 0;

private:
    std::weak_ptr<Agent> mp_target;
    Infantry_state m_infantry_state;
};
#endif // INFANTRY_H
