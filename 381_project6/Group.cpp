#include "Group.h"
#include "Agent.h"
#include "Utility.h"
#include "Geometry.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::for_each;
using std::bind; using namespace std::placeholders;
using std::shared_ptr;


// How far away from the destination point each member should be when a move
// command is given to a group with multiple members
constexpr double kGROUP_MOVE_OFFSET_MAGNITUDE = 0.5;

Group::Group(const string& name_) : m_name(name_)
{
}

bool Group::add_agent_helper(std::shared_ptr<Agent> agent) {
    // Attempt to insert agent
    auto return_val = m_members.insert(agent);

    // If agent was inserted then add this Group as a Death_observer
    if (return_val.second) {
        agent->add_to_my_groups(shared_from_this());
    }

    // Returns true if agent was added, false if agent was already present
    return return_val.second;
}

void Group::add_agent(std::shared_ptr<Agent> agent) {
    bool was_added = add_agent_helper(agent);

    // If return val holds 'false' then agent was already present in Group
    if (!was_added) {
        throw Error("Agent already a member of that Group!");
    }

    cout << "Group " << m_name << ":  " << agent->get_name() << " added" << endl;
}

bool Group::remove_agent_helper(std::shared_ptr<Agent> agent) {
    // Try to find the Agent in the Group
    auto iter = m_members.find(agent);

    // return false if agent is not a member of this Group
    if (iter == m_members.end()) {
        return false;
    }

    // Remove Agent from Group
    m_members.erase(iter);

    // Indicate successful removal of agent from group
    return true;
}

void Group::remove_agent(std::shared_ptr<Agent> agent) {
    bool was_removed = remove_agent_helper(agent);

    // Throw Error if Agent is not in this Group
    if (!was_removed) {
        throw Error("Agent not a member of that group!");
    }

    // Remove this Group from agent's death observers
    agent->remove_from_my_groups(shared_from_this());

    cout << "Group " << m_name << ":  " << agent->get_name() << " removed" << endl;
}

void Group::add_group(shared_ptr<Group> other_group) {
    for_each(other_group->m_members.begin(), other_group->m_members.end(),
        bind(&Group::add_agent_helper, this, _1));

    cout << "Group " << m_name << ":  group " << other_group->m_name << " added" << endl;
}

void Group::remove_group(shared_ptr<Group> other_group) {
    for_each(other_group->m_members.begin(), other_group->m_members.end(),
        bind(&Group::remove_agent_helper, this, _1));

    cout << "Group " << m_name << ":  group " << other_group->m_name << " removed" << endl;
}

void Group::clean_up_dead_agents() {
    auto iter = m_members.begin();

    // Remove each dead Agent from the members container
    while (iter != m_members.end()) {
        // If Agent is dead, remove it
        if (!iter->get()->is_alive()) {
            // Safely increment iterator
            iter = m_members.erase(iter);
            continue;
        }

        iter++;
    }
}

void Group::disband() {
    clean_up_dead_agents();

    // Tell all members to remove this Group from their Groups containers
    const shared_ptr<Group>& this_ptr = shared_from_this();
    for_each(m_members.begin(), m_members.end(),
        [&this_ptr](Group_members_t::value_type p){ p->remove_from_my_groups(this_ptr); });

    m_members.clear();
}

// Returns approximate location of the group as a whole
// Assumes group member list contains only living Agents
Point Group::calculate_location() const {
    // If group has no members then return a Point (0.0, 0.0)
    if (m_members.empty()) {
        return Point(0.0, 0.0);
    }

    // Accumlators for the new locations coordinates
    double new_px = 0.0;
    double new_py = 0.0;

    // Accumulate the values of the locations of all members of the group
    for (auto p : m_members) {
        const Point p_loc = p->get_location();
        new_px += p_loc.x;
        new_py += p_loc.y;
    }

    // Multiply accumulated values by weight to get average of all locations
    const double weight = 1.0 / static_cast<double>(m_members.size());
    new_px *= weight;
    new_py *= weight;

    // Return a Point that is the average of all the members' locations
    return Point(new_px, new_py);
}

void Group::move(const Point& destination) {
    clean_up_dead_agents();

    // Do nothing if Group is empty
    if (m_members.empty()) {
        return;
    }

    Point group_location = calculate_location();

    // Don't command the group to move if it is already there
    if (point_tolerance_compare_eq(group_location, destination)) {
        cout << "Group " << m_name << " is already there!" << endl;
        return;
    }

    // If Group only has one member then move that member to destination
    if (m_members.size() == 1) {
        m_members.begin()->get()->move_to(destination);
        return;
    }

    /* The group has multiple members and will command them all to move. The 
    locations each member is told to move will be offsets all equal distance 
    from the passed in destination. The first position filled will be offset
    from destination in the direction of the heading from the groups current location
    to the destination. The remaining positions will be assigned at even
    intervals in a counter-clockwise circle around the destination */

    // create a normalised offset heading from the group's current location
    // to the destination.
    Cartesian_vector offset_heading(group_location, destination);
    offset_heading.normalise();

    // Radians to rotate about destination per member
    const double theta_per_member = (2.0 * get_pi()) / static_cast<double>(m_members.size());

    // Rotation matrix for applying rotations
    const Rotation2D rotation_mat(theta_per_member);

    for (auto p : m_members) {
        p->move_to(destination + kGROUP_MOVE_OFFSET_MAGNITUDE * offset_heading);

        // Apply the per member rotation to the offset heading
        offset_heading = rotation_mat * offset_heading;
    }
}

void Group::stop() {
    clean_up_dead_agents();

    for (auto p : m_members) {
        p->stop();
    }
}

void Group::attack(std::shared_ptr<Agent> target) {
    clean_up_dead_agents();

    for (auto p : m_members) {
        p->start_attacking(target);
    }
}

void Group::work(std::shared_ptr<Structure> source, std::shared_ptr<Structure> destination) {
    clean_up_dead_agents();

    for (auto p : m_members) {
        p->start_working(source, destination);
    }
}

void Group::describe() {
    clean_up_dead_agents();

    // Print the number of members this Group has along with their names
    cout << "Group " << m_name << " has " << m_members.size() << " members:\n";
    for_each(m_members.begin(), m_members.end(),
        [](Group_members_t::value_type p){ cout << p->get_name() << endl; });
}

bool Group::is_agent_member(std::shared_ptr<Agent> query) {
    clean_up_dead_agents();
    return m_members.find(query) != m_members.end();
}

const string& Group::get_name() const {
    return m_name;
}

bool Group::operator==(const std::string& rhs_name) const {
    return m_name == rhs_name;
}
