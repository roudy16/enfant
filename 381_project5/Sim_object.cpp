#include "Sim_object.h"
#include "Utility.h"
#include <string>
#include <iostream>

using std::string;

Sim_object::Sim_object(const string& name_) : m_name(name_)
{
#ifdef PRINT_CTORS_DTORS
    cout << "Sim_object " << m_name << " constructed" << endl;
#endif
}

Sim_object::~Sim_object()
{
#ifdef PRINT_CTORS_DTORS
    cout << "Sim_object " << m_name << " destructed" << endl;
#endif
}
