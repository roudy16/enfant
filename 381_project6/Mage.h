#ifndef MAGE_H
#define MAGE_H

#include "Infantry.h"
#include <string>
#include <memory>

// Forward declaration
struct Point;

class Mage : public Infantry {
public:
    explicit Mage(const std::string& name_, const Point& location_);

    // TODO describe take hit behavior
    void take_hit(int attack_strength, std::shared_ptr<Agent> attacker_ptr) override;

    // disallow copy/move construction or assignment and default ctor
    Mage() = delete;
    Mage(const Mage&) = delete;
    Mage& operator= (const Mage&) = delete;
    Mage(Mage&&) = delete;
    Mage& operator= (Mage&&) = delete;

private:
    // do update tasks for Mage
    void do_update() override;
    // returns Mage's attack range
    double get_range() const override;
    // return string "Mage"
    const std::string& get_type_string() const override;

    // Mage teleports its range in the provided direction,
    // passed in direction must be normalised
    void teleport(Cartesian_vector direction);

    int m_charges;
    int m_recharge_cooldown_timer;
};

#endif // MAGE_H
