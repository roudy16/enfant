#include "String.h"
#include <utility>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    String::set_messages_wanted(true);
    String a = "Buffy";
    String b(std::move(a));
    String c;
    c = std::move(b);
    c = String("Charles");
    String d;
    d = "Pool";
    d[0] = 'C';
    d += " Party";
    d += " Palace";
    String e(" Funzy");
    c.clear();

    string str("Peeple");
    str[3] = '\0';

    d += e;
    cout << String::get_number() << "  " << String::get_total_allocation() << endl;
    cout << (e += e) << endl;
    cout << (e += e.c_str()) << endl;

    String s0("Sally");
    s0 += 'M';
    s0 = s0;
    const String s1(s0);
    cout << s1[5] << endl;

    return 0;
}