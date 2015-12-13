#include "Agent_factory.h"
#include "Agent.h"
#include "Archer.h"
#include "Mage.h"
#include "Soldier.h"
#include "Peasant.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <memory>

using std::string;
using std::shared_ptr; using std::make_shared;

shared_ptr<Agent> create_agent(const string& name, const string& type, Point location) {
    shared_ptr<Agent> new_agent_ptr;

    // Determine what type of Agent to create, throw Error if no such type
    if (type == "Peasant") {
        new_agent_ptr = make_shared<Peasant>(name, location);
    }
    else if (type == "Soldier") {
        new_agent_ptr = make_shared<Soldier>(name, location);
    }
    else if (type == "Archer") {
        new_agent_ptr = make_shared<Archer>(name, location);
    }
    else if (type == "Mage") {
        // TODO create the Mage bud
        new_agent_ptr = make_shared<Mage>(name, location);
    }
    else {
        throw Error("Trying to create agent of unknown type!");
    }

    return new_agent_ptr;
}
