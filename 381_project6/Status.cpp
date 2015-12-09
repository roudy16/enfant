#include "Status.h"
#include <iostream>

using std::string;
using std::cout; using std::endl;


Status::Status(const string& name) : View(name)
{
}

Status::~Status()
{
}

// if it exists, remove the object that has name from this Status 
void Status::update_remove(const string& name) {
    auto iter = m_objects.find(name);

    // Do nothing if name not found
    if (iter == m_objects.end()) {
        return;
    }

    // Remove found object
    m_objects.erase(iter);
}

void Status::clear() {
    m_objects.clear();
}

// Print object names followed by their associated status value
void Status::do_draw_body() {
    for (auto& obj : m_objects) {
        cout << obj.first << ": " << obj.second << endl;
    }
    cout << "--------------" << endl;
}

// update the status value of an object, if that object is not currently
// contained in this Status then add it.
void Status::update_status(const string& name, double val) {
    auto obj_iter = m_objects.find(name);

    if (obj_iter == m_objects.end()) {
        m_objects[name] = val;
    }
    else {
        obj_iter->second = val;
    }
}

