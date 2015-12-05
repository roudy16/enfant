#include "Town_Hall.h"
#include "Geometry.h"
#include "Utility.h"
#include "Model.h"
#include <string>
#include <cmath>
#include <iostream>

using std::string;
using std::cout; using std::endl;

// Initial value of Town_Hall food amount
constexpr double kTOWNHALL_INITIAL_FOOD_AMOUNT = 0.0;
// Default tax rate
constexpr double kTOWNHALL_TAX_RATE = 0.1;
// Minimum amount that can be withdrawn from a Town_Hall
constexpr double kTOWNHALL_MIN_WITHDRAW = 1.0;

Town_Hall::Town_Hall(const std::string& name_, Point location_)
    : Structure(name_, location_),
    m_food_amount(kTOWNHALL_INITIAL_FOOD_AMOUNT)
{
}

Town_Hall::~Town_Hall()
{
}

// deposit adds in the supplied amount
void Town_Hall::deposit(double deposit_amount) {
    m_food_amount += deposit_amount;
    Model::get_instance()->notify_amount(get_name(), m_food_amount);
}

// ask model to notify views of current state
void Town_Hall::broadcast_current_state() const {
    Structure::broadcast_current_state();
    Model::get_instance()->notify_amount(get_name(), m_food_amount);
}

// Return whichever is less, the request or (the amount on hand - 10%) (a "tax"),
// but amounts less than 1.0 are not supplied - the amount returned is zero.
// update the amount on hand by subtracting the amount returned.
double Town_Hall::withdraw(double amount_to_obtain) {
    double return_amount = fmin(amount_to_obtain,
                                m_food_amount - m_food_amount * kTOWNHALL_TAX_RATE);

    // Don't return amounts less than kTOWNHALL_MIN_WITHDRAW
    if (return_amount < kTOWNHALL_MIN_WITHDRAW) {
        return_amount = 0.0;
    }

    // Reduce amount on hand by amount to be returned
    m_food_amount -= return_amount ;
    Model::get_instance()->notify_amount(get_name(), m_food_amount);

    return return_amount;
}

// output information about the current state
void Town_Hall::describe() const {
    cout << "Town_Hall ";
    Structure::describe();
    cout << "   Contains " << m_food_amount << endl;
}
