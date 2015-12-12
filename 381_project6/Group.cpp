#include "Group.h"
#include "Agent.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::for_each;
using std::shared_ptr; using std::static_pointer_cast;

Group::Group(const string& name_) : m_name(name_)
{
}

void Group::update_on_death(const std::string& name) {
    auto iter = m_members.find(name);
    assert(iter != m_members.end());

    remove_agent(iter->second);
}

void Group::add_agent(std::shared_ptr<Agent> agent) {
    auto iter = m_members.find(agent->get_name());

    if (iter != m_members.end()) {
        throw Error("Agent already a member of Group");
    }

    m_members[agent->get_name()] = agent;

    cout << "Group " << m_name << ": agent " << agent->get_name() << " added" << endl;
}

void Group::remove_agent(std::shared_ptr<Agent> agent) {
    // Try to find the Agent in the Group
    auto iter = m_members.find(agent->get_name());

    // Throw Error if Agent is not in this Group
    if (iter == m_members.end()) {
        throw Error("Agent not a member of that group!");
    }

    // Remove Agent from Group
    m_members.erase(iter);
}

void Group::disband() {
    shared_ptr<Death_observer> this_ptr = static_pointer_cast<Death_observer>(shared_from_this());
    for_each(m_members.begin(), m_members.end(),
        [&this_ptr](Group_members_t::value_type& p){ p.second->detach_death_observer(this_ptr); });

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
        [](const Group_members_t::value_type& p){ cout << p.second->get_name() << '\n'; });
    cout << "Total members: " << m_members.size() << endl;
    // TODO add formation if I do that
}

const string& Group::get_name() const {
    return m_name;
}

bool Group::operator==(const std::string& rhs_name) const {
    return m_name == rhs_name;
}
