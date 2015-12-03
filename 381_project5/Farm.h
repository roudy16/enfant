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
    Farm(const std::string& name_, Point location_);

    ~Farm() override;

    // update adds the production amount to the stored amount
    void update() override;

    // output information about the current state
    void describe() const override;

    // returns the specified amount, or the remaining amount, whichever is less,
    // and deducts that amount from the amount on hand
    double withdraw(double amount_to_get) override;

    double get_amount() const override;

    // disallow copy/move construction or assignment and default ctor
    Farm() = delete;
    Farm(const Farm&) = delete;
    Farm& operator= (const Farm&)  = delete;
    Farm(Farm&&) = delete;
    Farm& operator= (Farm&&) = delete;

private:
    double m_food_amount;
    double m_production_rate;
};

#endif // FARM_H
