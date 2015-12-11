#include "Group.h"
#include <string>

using std::string;

Group::Group(const string& name_) : m_name(name_)
{
}

void Group::update_on_death(const std::string& name) {

}

void Group::add_agent(std::shared_ptr<Agent> agent) {

}

void Group::remove_agent(std::shared_ptr<Agent> agent) {

}

bool Group::operator==(const std::string& rhs_name) const {
    return m_name == rhs_name;
}
