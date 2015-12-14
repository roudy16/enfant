#include "Agent.h"
#include "Geometry.h"
#include <string>
#include <memory>
#include <set>


class Group : public std::enable_shared_from_this<Group> {
public:
    explicit Group(const std::string& name);

    // Alerts all members to remove this Group from their own Groups container
    // then removes all members from Group
    void disband();

    // Prints the Group's name, number of members, and lists the names of the members
    void describe();

    // Interface for commanding all Group members
    void move(const Point& destination);
    void stop();
    void attack(std::shared_ptr<Agent> target);
    void work(std::shared_ptr<Structure> source, std::shared_ptr<Structure> destination);

    // Interface for add/removing Agents/Groups
    void add_agent(std::shared_ptr<Agent> agent);
    void remove_agent(std::shared_ptr<Agent> agent);
    void add_group(std::shared_ptr<Group> other_group);
    void remove_group(std::shared_ptr<Group> other_group);

    // Returns true is query Agent is a member of this Group
    bool is_agent_member(std::shared_ptr<Agent> query);

    // Returns name of the Groupo
    const std::string& get_name() const;

    // Groups compare equal to strings that match the Group's name
    bool operator==(const std::string& rhs_name) const;

    // disallow copy/move construction or assignment and default ctor
    Group() = delete;
    Group(const Group&) = delete;
    Group& operator= (const Group&)  = delete;
    Group(Group&&) = delete;
    Group& operator= (Group&&) = delete;

private:
    // Returns true if agent was added, false if agent was already present
    // These two cases are the only possible outcomes
    bool add_agent_helper(std::shared_ptr<Agent> agent);
    // Returns true if agent was successfully removed from the Group, false if
    // is not present in Group and thus cannot be removed
    bool remove_agent_helper(std::shared_ptr<Agent> agent);

    // Removes dead Agents from the Group
    void clean_up_dead_agents();

    // Returns approximate location of the group as a whole
    Point calculate_location() const;

    // Comparator used to order members by name
    struct Members_comp {
        bool operator()(std::shared_ptr<Agent> lhs, std::shared_ptr<Agent> rhs) const {
            return lhs->get_name() < rhs->get_name();
        }
    };

    using Group_members_t = std::set<std::shared_ptr<Agent>, Members_comp>;

    Group_members_t   m_members;
    const std::string m_name;
};

