#include "Town_Hall.h"
#include "Geometry.h"
#include <string>
#include <iostream>

using namespace std;

const double kTOWNHALL_INITIAL_FOOD_AMOUNT = 0.0;
const double kTOWNHALL_INITIAL_TAX_RATE = 0.1;

Town_Hall::Town_Hall(const std::string& name_, Point location_)
    : Structure(name_, location_), m_food_amount(kTOWNHALL_INITIAL_FOOD_AMOUNT)
{
    cout << "Town_Hall " << get_name() << " constructed" << endl;
}

Town_Hall::~Town_Hall() {
    cout << "Town_Hall " << get_name() << " destructed" << endl;
}

// deposit adds in the supplied amount
void Town_Hall::deposit(double deposit_amount) {
    m_food_amount += deposit_amount;
}

// Return whichever is less, the request or (the amount on hand - 10%) (a "tax"),
// but amounts less than 1.0 are not supplied - the amount returned is zero.
// update the amount on hand by subtracting the amount returned.
double Town_Hall::withdraw(double amount_to_obtain) {
    double return_amount = m_food_amount - m_food_amount * m_tax_rate;

    // Don't return amounts less than 1.0
    if (return_amount < 1.0) {
        return_amount = 0.0;
    }

    // Reduce amount on hand by amount to be returned
    m_food_amount -= return_amount;
    return return_amount;
}

// output information about the current state
void Town_Hall::describe() const {
    cout << "Town_Hall ";
    Structure::describe();
}
