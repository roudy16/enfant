#ifndef ARCHER_H
#define ARCHER_H

#include "Infantry.h"
#include <memory>
#include <string>


class Archer : public Infantry {
public:
    explicit Archer(const std::string& name_, const Point& location_);

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
    // performs Archer attack behavior
    void attack_helper();
    // returns Archer's attack range
    double get_range() const override;
    // return string "Archer"
    const std::string& get_type_string() const override;
};

#endif // ARCHER_H
