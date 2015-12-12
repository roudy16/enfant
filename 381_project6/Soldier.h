#ifndef SOLDIER_H
#define SOLDIER_H

#include "Infantry.h"
#include <memory>
#include <string>


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
    Soldier& operator= (const Soldier&) = delete;
    Soldier(Soldier&&) = delete;
    Soldier& operator= (Soldier&&) = delete;

private:
    // do update tasks for Soldier
    void do_update() override;
    // returns Soldier's attack range
    double get_range() const noexcept override;
    // returns string "Soldier"
    const std::string& get_type_string() const noexcept override;
};

#endif // SOLDIER_H
