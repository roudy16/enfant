#include "Death_observer.h"
#include "Agent.h"
#include <string>
#include <memory>
#include <set>

// Forward declaration
struct Point;

class Group : public Death_observer, public std::enable_shared_from_this<Group> {
public:
    explicit Group(const std::string& name);

    void update_on_death(std::shared_ptr<Agent>) override;

    void disband();
    void move(const Point& destination);
    void stop();
    void attack(std::shared_ptr<Agent> target);

    void add_agent(std::shared_ptr<Agent> agent);
    void remove_agent(std::shared_ptr<Agent> agent);

    void describe() const;

    const std::string& get_name() const;

    bool operator==(const std::string& rhs_name) const;

    // disallow copy/move construction or assignment and default ctor
    Group() = delete;
    Group(const Group&) = delete;
    Group& operator= (const Group&)  = delete;
    Group(Group&&) = delete;
    Group& operator= (Group&&) = delete;

private:
    // Comparator used to order members by name
    struct Members_comp {
        bool operator()(std::shared_ptr<Agent> lhs, std::shared_ptr<Agent> rhs) {
            return lhs->get_name() < rhs->get_name();
        }
    };

    using Group_members_t = std::set<std::shared_ptr<Agent>, Members_comp>;
    enum class Formation { LINE, COLUMN, WEDGE };

    Group_members_t   m_members;
    const std::string m_name;
    Formation         m_formation;
};

