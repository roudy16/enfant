#ifndef DEATH_OBSERVER_H
#define DEATH_OBSERVER_H

#include <memory>

// Forward declaration
class Agent;

class Death_observer {
public:
    virtual ~Death_observer();
    // called when a 
    virtual void update_on_death(std::shared_ptr<Agent> agent_ptr) = 0;
};

#endif // DEATH_OBSERVER_H
