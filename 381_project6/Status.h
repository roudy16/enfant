#ifndef STATUS_H
#define STATUS_H

#include "View.h"


/*
    Status is an abstract base class of Views that can display numerical information
    related to an object.
*/
class Status : public View {
public:
    // Make Status an abstract class
    virtual ~Status() = 0;

    // Remove named object from Status objects container
    void update_remove(const std::string& name) override;
    // Remove all objects
    void clear() override;

protected:
    Status(const std::string& name);

    // Updates an objects status value, adds object name to container if the
    // object is not already there.
    void update_status(const std::string& name, double val);

private:
    using Status_objects_t = std::map<std::string, double>;

    // Hook for drawing
    void do_draw_body() override;

    Status_objects_t m_objects;
};

#endif // STATUS_H
