#include "Comparators.h"
#include "Geometry.h"
#include "Sim_object.h"
#include "Agent.h"
#include <memory>

using std::shared_ptr;

Closest_to_obj::Closest_to_obj(shared_ptr<Sim_object> obj_ptr)
    : m_location(obj_ptr->get_location()), m_name(obj_ptr->get_name())
{
}

bool Closest_to_obj::closest_comp_helper(shared_ptr<Sim_object> lhs, shared_ptr<Sim_object> rhs) const {
    // Check if either argument is the object used to init
    // Object used to init always evaluates greater than any other
    if (lhs->get_name() == m_name) {
        return false;
    }
    if (rhs->get_name() == m_name) {
        return true;
    }

    // get distances from args to init object
    double dist_to_lhs = cartesian_distance(m_location, lhs->get_location());
    double dist_to_rhs = cartesian_distance(m_location, rhs->get_location());

    // resolves distance ties with name comparison
    if (dist_to_lhs == dist_to_rhs) {
        return lhs->get_name() < rhs->get_name();
    }

    // return true if lhs is closer than rhs
    return dist_to_lhs < dist_to_rhs;
}

Closest_hostile_to_agent::Closest_hostile_to_agent(shared_ptr<Agent> agent) 
    : Closest_to_obj(agent), m_agent(agent)
{
}

bool Closest_hostile_to_agent::operator()(Model::Agents_t::value_type& lhs,
                Model::Agents_t::value_type& rhs) const 
{
    if (m_agent->agents_share_group(lhs.second)) {
        return false;
    }

    return closest_comp_helper(lhs.second, rhs.second);
}
