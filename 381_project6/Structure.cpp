#include "Structure.h"
#include "Utility.h"
#include "Model.h"
#include "Geometry.h"
#include <iostream>
#include <string>

using std::string;
using std::cout; using std::endl;

Structure::Structure(string name, Point location) 
    : Sim_object(name), m_location(location)
{
}

Structure::~Structure()
{
}

void Structure::update()
{
}

// output information about the current state
void Structure::describe() const {
    cout << get_name() << " at " << m_location << endl;
}

// ask model to notify views of current state
void Structure::broadcast_current_state() const {
    Model::get_instance()->notify_location(get_name(), get_location());
}

double Structure::withdraw(double amount_to_get) {
    return 0.0; 
}

void Structure::deposit(double amount_to_give)
{
}
