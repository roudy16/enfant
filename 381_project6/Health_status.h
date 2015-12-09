#ifndef HEALTH_STATUS_H
#define HEALTH_STATUS_H

#include "Status.h"
#include <string>

/*
    TODO Class description
*/

class Health_status : public Status {
public:
    explicit Health_status();

    // Updates an objects health value, adds object name to container if the
    // object is not already there.
    void update_health(const std::string& name, double health) override;

    Health_status(const Health_status&) = delete;
    Health_status& operator= (const Health_status&) = delete;
    Health_status(Health_status&&) = delete;
    Health_status& operator= (Health_status&&) = delete;

private:
    // Hook for drawing
    void do_draw_header() override;
};
#endif // HEALTH_STATUS_H
