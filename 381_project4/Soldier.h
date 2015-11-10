#ifndef SOLDIER_H
#define SOLDIER_H

#include "Agent.h"

/*
A Soldier is an Agent that has attack and defense behaviors. It can be commanded
to start attacking another Agent and will continue the attack as long as 
it is alive and the target is alive and in range. If attacked, the Soldier will
start attacking its attacker.
*/
    
/* 
*** This skeleton file shows the required public interface for the class, which you may not modify. 
If no protected members are shown, there must be none in your version. 
If any protected or private members are shown here, then your class must also have them and use them as intended.
You must delete this comment and all other comments that start with "***".
*/

class Soldier : public Agent {
public:

    // *** define as specified
    Soldier(const std::string& name_, Point location_);

    ~Soldier();

    // update implements Soldier behavior
    void update() override;

    // Make this Soldier start attacking the target Agent.
    // Throws an exception if the target is the same as this Agent,
    // is out of range, or is not alive.
    void start_attacking(Agent* target_ptr) override;

    // Overrides Agent's take_hit to counterattack when attacked.
    void take_hit(int attack_strength, Agent* attacker_ptr) override;

    // Overrides Agent's stop to print a message
    void stop() override;

    // output information about the current state
    void describe() const override;
};

#endif // SOLDIER_H
