#include "Structure.h"
#include "Model.h"
#include <iostream>

using namespace std;

// output information about the current state
void Structure::describe() {
    cout << m_name << " at " << m_location << endl;
}

// ask model to notify views of current state
void Structure::broadcast_current_state() {
    g_Model_ptr->notify_location(m_name, m_location);
}


