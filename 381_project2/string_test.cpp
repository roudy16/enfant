#include "String.h"
#include <utility>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    String::set_messages_wanted(true);
    String a = "Buffy";
    String b = std::move(a);
    String c(b);
    c = String("Charles");
    String d;
    d = "Pool";
    d[0] = 'C';
    d += " Party";
    d += " Palace";
    String e(" Funzy");
    c.clear();

    string str("Peeple");
    cout << str << " " << str.length() << endl;
    str[3] = '\0';
    cout << str << " " << str.length() << endl;

    cout << a << " " << a.get_allocation() << endl;
    cout << b << " " << b.get_allocation() << endl;
    cout << d << " " << d.get_allocation() << endl;
    d += e;
    cout << d << " " << d.get_allocation() << endl;
    cout << d[3] << endl;
    cin >> c;
    cin >> d;
    cout << c << " " << c.get_allocation() << endl;
    cout << d << " " << d.get_allocation() << endl;
    cout << String::get_number() << "  " << String::get_total_allocation() << endl;
    cout << "a: " << a.get_allocation() << endl;
    cout << "b: " << b.get_allocation() << endl;
    cout << "c: " << c.get_allocation() << endl;
    cout << "d: " << d.get_allocation() << endl;
    cout << "e: " << e.get_allocation() << endl;
    cout << (e += e) << endl;

    return 0;
}