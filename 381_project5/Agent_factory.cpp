#include "Agent_factory.h"
#include "Agent.h"
#include "Soldier.h"
#include "Peasant.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <memory>

using namespace std;

shared_ptr<Agent> create_agent(const string& name, const string& type, Point location) {
    shared_ptr<Agent> new_agent_ptr;

    // Determine what type of Agent to create, throw Error if no such type
    if (type == "Peasant") {
        new_agent_ptr = static_pointer_cast<Agent>(make_shared<Peasant>(name, location));
    }
    else if (type == "Soldier") {
        new_agent_ptr = static_pointer_cast<Agent>(make_shared<Soldier>(name, location));
    }
    else {
        throw Error("Trying to create agent of unknown type!");
    }

    return new_agent_ptr;
}
