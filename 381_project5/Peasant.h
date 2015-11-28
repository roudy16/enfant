#ifndef PEASANT_H
#define PEASANT_H

#include "Agent.h"
#include <string>
#include <memory>

/* 
A Peasant is an Agent that can move food between Structures. It can be commanded to
start_working, whereupon it moves to the source, picks up food, returns to destination,
deposits the food, returns to source.  If picks up zero food at the source, it waits there
and tries again on the next update. 
If commanded to move_to somewhere, it stops working, and goes there.
*/

// Forward declaration of Structure
class Structure;

class Peasant : public Agent {
public:
    Peasant(const std::string& name_, Point location_);

    ~Peasant();

    // implement Peasant behavior
    void update() override;

    // overridden to suspend working behavior
    void move_to(Point dest) override;

    // stop moving and working
    void stop() override;

    // starts the working process
    // Throws an exception if the source is the same as the destination.
    void start_working(std::shared_ptr<Structure> source_, 
                       std::shared_ptr<Structure> destination_) override;

    // output information about the current state
    void describe() const override;

private:
    enum class Peasant_State { NOT_WORKING, DEPOSITING, COLLECTING, INBOUND, OUTBOUND };

    // Returns true if the Peasant is in any 'working' state
    bool is_working();

    // sets Peasant state to NOT_WORKING and forgets source and destination Structures
    void forget_work();

    // Peasant forgets work and outputs stop message
    void stop_working();

    std::shared_ptr<Structure> m_source;
    std::shared_ptr<Structure> m_destination;
    double m_amount;
    Peasant_State m_peasant_state;

    // disallow copy/move construction or assignment and default ctor
    Peasant() = delete;
    Peasant(const Peasant&) = delete;
    Peasant& operator= (const Peasant&)  = delete;
    Peasant(Peasant&&) = delete;
    Peasant& operator= (Peasant&&) = delete;
};

#endif // PEASANT_H
