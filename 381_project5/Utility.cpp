#include "Utility.h"
#include "Sim_object.h"

using namespace std;

Closest_to_obj::Closest_to_obj(shared_ptr<Sim_object>& obj_ptr)
    : m_location(obj_ptr->get_location()), m_name(obj_ptr->get_name())
{
}
