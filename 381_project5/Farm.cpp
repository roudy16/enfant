#include "Farm.h"
#include "Geometry.h"
#include "Model.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <cmath>

using std::string;
using std::cout; using std::endl;

// Initial Farm food amount
constexpr double kFARM_INITIAL_FOOD_AMOUNT = 50.0;
// Farm production rate
constexpr double kFARM_PRODUCTION_RATE = 2.0;

Farm::Farm(const string& name_, Point location_)
    : Structure(name_, location_), m_food_amount(kFARM_INITIAL_FOOD_AMOUNT)
{
}

Farm::~Farm()
{
}

// returns the specified amount, or the remaining amount, whichever is less,
// and deducts that amount from the amount on hand
double Farm::withdraw(double amount_to_get) {
    // return the max between the amount requested and the amount on hand
    double return_amount = fmin(amount_to_get, m_food_amount);

    // remove return amount from the amount on hand and have Model notify Views
    m_food_amount -= return_amount;
    Model::get_instance()->notify_amount(get_name(), m_food_amount);

    return return_amount;
}

// ask model to notify views of current state
void Farm::broadcast_current_state() const {
    Structure::broadcast_current_state();
    Model::get_instance()->notify_amount(get_name(), m_food_amount);
}


// update adds the production amount to the stored amount and tells Model to
// notify Views
void Farm::update() {
    m_food_amount += kFARM_PRODUCTION_RATE;
    Model::get_instance()->notify_amount(get_name(), m_food_amount);
    cout << "Farm " << get_name() << " now has " << m_food_amount << endl;
}

// output information about the current state
void Farm::describe() const {
    cout << "Farm ";
    Structure::describe();
    cout << "   Food available: " << m_food_amount << endl;
}
