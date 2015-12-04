#include "Town_Hall.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

constexpr double kTOWNHALL_INITIAL_FOOD_AMOUNT = 0.0;
constexpr double kTOWNHALL_INITIAL_TAX_RATE = 0.1;
constexpr double kTOWNHALL_MIN_WITHDRAW = 1.0;

Town_Hall::Town_Hall(const std::string& name_, Point location_)
    : Structure(name_, location_),
    m_food_amount(kTOWNHALL_INITIAL_FOOD_AMOUNT),
    m_tax_rate(kTOWNHALL_INITIAL_TAX_RATE)
{
#ifdef PRINT_CTORS_DTORS
    cout << "Town_Hall " << get_name() << " constructed" << endl;
#endif
}

Town_Hall::~Town_Hall() {
#ifdef PRINT_CTORS_DTORS
    cout << "Town_Hall " << get_name() << " destructed" << endl;
#endif
}

// deposit adds in the supplied amount
void Town_Hall::deposit(double deposit_amount) {
    m_food_amount += deposit_amount;
}

// Return whichever is less, the request or (the amount on hand - 10%) (a "tax"),
// but amounts less than 1.0 are not supplied - the amount returned is zero.
// update the amount on hand by subtracting the amount returned.
double Town_Hall::withdraw(double amount_to_obtain) {
    double return_amount = fmin(amount_to_obtain,
                                m_food_amount - m_food_amount * m_tax_rate);

    // Don't return amounts less than kTOWNHALL_MIN_WITHDRAW
    if (return_amount < kTOWNHALL_MIN_WITHDRAW) {
        return_amount = 0.0;
    }

    // Reduce amount on hand by amount to be returned
    m_food_amount -= return_amount ;
    return return_amount;
}

// output information about the current state
void Town_Hall::describe() const {
    cout << "Town_Hall ";
    Structure::describe();
    cout << "   Contains " << m_food_amount << endl;
}
