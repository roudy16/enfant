#include "Agent_factory.h"
#include "Agent.h"
#include "Soldier.h"
#include "Peasant.h"
#include "Geometry.h"
#include "Utility.h"

Agent * create_agent(const std::string& name, const std::string& type, Point location) {
    Agent* new_agent_ptr = nullptr;

    // Determine what type of Agent to create, throw Error if no such type
    if (type == "Peasant") {
        new_agent_ptr = new Peasant(name, location);
    }
    else if (type == "Soldier") {
        new_agent_ptr = new Soldier(name, location);
    }
    else {
        throw Error("Trying to create agent of unknown type!");
    }

    return new_agent_ptr;
}
