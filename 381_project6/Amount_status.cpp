#include "Amount_status.h"
#include <iostream>
#include <string>

using std::string;
using std::cout; using std::endl;

Amount_status::Amount_status() : Status("amounts")
{
}

void Amount_status::update_amount(const string& name, double amount) {
    update_status(name, amount);
}

void Amount_status::do_draw_header() {
    cout << "Current Amounts:\n--------------" << endl;
}


