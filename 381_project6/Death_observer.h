#ifndef DEATH_OBSERVER_H
#define DEATH_OBSERVER_H

#include <string>


class Death_observer {
public:
    virtual ~Death_observer();
    // called when a 
    virtual void update_on_death(const std::string& name) = 0;
};

#endif // DEATH_OBSERVER_H
