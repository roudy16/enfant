#include "Death_observer.h"
#include <string>
#include <map>
#include <memory>

// Forward declaration
class Agent;

class Group : public Death_observer, public std::enable_shared_from_this<Group> {
public:
    explicit Group(const std::string& name);

    void update_on_death(const std::string& name) override;

    void add_agent(std::shared_ptr<Agent> agent);
    void remove_agent(std::shared_ptr<Agent> agent);

    bool operator==(const std::string& rhs_name) const;

private:
    enum class Formation { LINE, COLUMN, WEDGE };

    std::map<const std::string, std::shared_ptr<Agent>> m_agents;
    const std::string m_name;
    Formation m_formation;
};

