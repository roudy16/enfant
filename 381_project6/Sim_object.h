#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H

#include "Geometry.h"
#include <string>
#include <memory>

// TODO
#include "Utility.h"

/* The Sim_object class provides the interface for all of simulation objects. 
It also stores the object's name, and has pure virtual accessor functions for 
the object's position and other information. */

class Sim_object : public std::enable_shared_from_this<Sim_object> {
public:
    Sim_object(const std::string& name_);
    virtual ~Sim_object();

    const std::string& get_name() const noexcept { return m_name; };

    // ask model to notify views of current state
    virtual void broadcast_current_state() const = 0;
    virtual Point get_location() const = 0;
    virtual void describe() const = 0;
    virtual void update() = 0;

private:
    const std::string m_name;
};

#endif // SIM_OBJECT_H
