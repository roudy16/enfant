#include "String.h"
#include <utility>
#include <iostream>

using namespace std;

int main() {
    String a = "Buffy";
    cout << a.c_str() << endl;
    String b = std::move(a);
    cout << a.c_str() << " " << a.get_allocation() << endl;
    cout << b.c_str() << " " << b.get_allocation() << endl;
    cout << String::get_number() << "  " << String::get_total_allocation() << endl;
    return 0;
}