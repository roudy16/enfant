#include "Sim_object.h"
#include <string>
#include <iostream>

using namespace std;

Sim_object::Sim_object(const string& name_) : m_name(name_)
{
    cout << "Sim_object " << m_name << " constructed" << endl;
}

Sim_object::~Sim_object()
{
    cout << "Sim_object " << m_name << " destructed" << endl;
}

const string& Sim_object::get_name() const {
    return m_name;
}
