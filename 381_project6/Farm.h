#ifndef FARM_H
#define FARM_H

#include "Structure.h"
#include <string>

/*
A Farm is a Structure that when updated, increments the amount of food on hand
by the production rate amount.
Food can be withdrawn, but no provision is made for depositing any.
*/

struct Point;

class Farm : public Structure {
public:
    explicit Farm(const std::string& name_, const Point& location_);

    ~Farm() override;

    // update adds the production amount to the stored amount
    void update() override;

    // output information about the current state
    void describe() const override;

    // ask model to notify views of current state
    void broadcast_current_state() const override;

    // returns the specified amount, or the remaining amount, whichever is less,
    // and deducts that amount from the amount on hand
    double withdraw(double amount_to_get) override;

    // disallow copy/move construction or assignment and default ctor
    Farm() = delete;
    Farm(const Farm&) = delete;
    Farm& operator= (const Farm&)  = delete;
    Farm(Farm&&) = delete;
    Farm& operator= (Farm&&) = delete;

private:
    double m_food_amount;
};

#endif // FARM_H
