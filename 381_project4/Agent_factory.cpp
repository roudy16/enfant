#include "Agent_factory.h"
#include "Agent.h"
#include "Soldier.h"
#include "Peasant.h"
#include "Geometry.h"
#include "Utility.h"

Agent * create_agent(const std::string& name, const std::string& type, Point location) {
    Agent* new_agent_ptr = nullptr;

    if (type.compare("Peasant") == 0) {
        new_agent_ptr = new Peasant(name, location);
    }
    else if (type.compare("Soldier") == 0) {
        new_agent_ptr = new Soldier(name, location);
    }
    else {
        throw Error("Trying to create agent of unknown type!");
    }

    return new_agent_ptr;
}
