#include "Health_status.h"
#include <iostream>

using std::string;
using std::cout; using std::endl;

Health_status::Health_status() : Status("health") 
{
}

void Health_status::update_health(const string& name, double health) {
    update_status(name, health);
}

void Health_status::do_draw_header() {
    cout << "Current Health:\n--------------" << endl;
}
