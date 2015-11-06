#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "Sim_object.h"
#include "Geometry.h"
#include <string>

/* A Structure is a Sim_object with a location and interface to derived types */

class Structure : public Sim_object {
public:
    Structure(std::string name, Point location);
    virtual ~Structure() = 0;

    // *** declare and define here appropriately
    Point get_location() { return m_location; }

    // *** declare and define the following functions as specified
    virtual void update() {};

    // output information about the current state
    virtual void describe();

    // ask model to notify views of current state
    virtual void broadcast_current_state();

    // fat interface for derived types
    virtual double withdraw(double amount_to_get);
    virtual void deposit(double amount_to_give);

private:
    std::string m_name;
    Point m_location;
};

#endif // STRUCTURE_H
