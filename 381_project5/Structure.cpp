#include "Structure.h"
#include "Model.h"
#include <iostream>
#include <string>
#include <memory>

using namespace std;

Structure::Structure(string name, Point location) 
    : Sim_object(name), m_location(location)
{
    cout << "Structure " << get_name() << " constructed" << endl;
}

Structure::~Structure() {
    cout << "Structure " << get_name() << " destructed" << endl;
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
    Model::get_instance()->notify_amount(get_name(), get_amount());
}

double Structure::withdraw(double amount_to_get) {
    return 0.0; 
}

void Structure::deposit(double amount_to_give)
{
}

double Structure::get_amount() const {
    return 0.0;
}
