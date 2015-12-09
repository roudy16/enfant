#include "Status.h"
#include <string>

/*
    TODO Class description
*/

class Amount_status : public Status {
public:
    explicit Amount_status();

    // Updates an objects amount value, adds object name to container if the
    // object is not already there.
    void update_amount(const std::string& name, double amount) override;

    Amount_status(const Amount_status&) = delete;
    Amount_status& operator= (const Amount_status&) = delete;
    Amount_status(Amount_status&&) = delete;
    Amount_status& operator= (Amount_status&&) = delete;

private:
    //Hook for drawing
    void do_draw_header() override;
};
