#include "Group.h"
#include "Agent.h"
#include "Utility.h"
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
using std::shared_ptr; using std::static_pointer_cast;

Group::Group(const string& name_) : m_name(name_)
{
}

void Group::update_on_death(shared_ptr<Agent> agent_ptr) {
    auto iter = m_members.find(agent_ptr);
    assert(iter != m_members.end());

    m_members.erase(iter);
}

bool Group::add_agent_helper(std::shared_ptr<Agent> agent) {
    // Attempt to insert agent
    auto return_val = m_members.insert(agent);

    // If agent was inserted then add this Group as a Death_observer
    if (return_val.second) {
        agent->attach_death_observer(static_pointer_cast<Death_observer>(shared_from_this()));
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
    agent->detach_death_observer(static_pointer_cast<Death_observer>(shared_from_this()));

    cout << "Group " << m_name << ":  " << agent->get_name() << " removed" << endl;
}

void Group::add_group(Group& other_group) {
    for_each(other_group.m_members.begin(), other_group.m_members.end(),
        bind(&Group::add_agent_helper, this, _1));

    cout << "Group " << m_name << ":  group " << other_group.m_name << " added" << endl;
}

void Group::remove_group(Group& other_group) {
    for_each(other_group.m_members.begin(), other_group.m_members.end(),
        bind(&Group::remove_agent_helper, this, _1));

    cout << "Group " << m_name << ":  group " << other_group.m_name << " removed" << endl;
}

void Group::disband() {
    shared_ptr<Death_observer> this_ptr = static_pointer_cast<Death_observer>(shared_from_this());
    for_each(m_members.begin(), m_members.end(),
        [&this_ptr](const Group_members_t::value_type& p){ p->detach_death_observer(this_ptr); });

    m_members.clear();
}

void Group::move(const Point& destination) {

}

void Group::stop() {

}

void Group::attack(std::shared_ptr<Agent> target) {

}

void Group::describe() const {
    cout << "Group " << m_name + ":\n";
    for_each(m_members.begin(), m_members.end(),
        [](const Group_members_t::value_type& p){ cout << p->get_name() << '\n'; });
    cout << "Total members: " << m_members.size() << endl;
    // TODO add formation if I do that
}

const string& Group::get_name() const {
    return m_name;
}

bool Group::operator==(const std::string& rhs_name) const {
    return m_name == rhs_name;
}
