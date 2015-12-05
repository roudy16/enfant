#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "Sim_object.h"
#include "Geometry.h"
#include <string>

/* A Structure is a Sim_object with a location and interface to derived types */

class Structure : public Sim_object {
public:
    // disallow copy/move construction or assignment and default ctor
    Structure() = delete;
    Structure(const Structure&) = delete;
    Structure& operator= (const Structure&)  = delete;
    Structure(Structure&&) = delete;
    Structure& operator= (Structure&&) = delete;

    Structure(std::string name, Point location);
    // Make this an abstract class
    virtual ~Structure() = 0;

    Point get_location() const override { return m_location; }

    void update() override;

    // output information about the current state
    void describe() const override;

    // ask model to notify views of current state
    void broadcast_current_state() const override;

    // fat interface for derived types
    virtual double withdraw(double amount_to_get);
    virtual void deposit(double amount_to_give);

private:
    Point m_location;
};

#endif // STRUCTURE_H
