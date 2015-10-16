#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include <iostream>

using namespace std;

void add_num(int& num){
    num += 10;
}

void add_num_arg(int& num, int to_add){
    num += to_add;
}

template <typename T>
void print_ol(const T& data, ostream& os){
    os << data << endl;
}

int main() {
    Ordered_list<int> il;
    auto it = il.begin();
    cout << &it << endl;

    il.insert(1);
    il.insert(0);
    il.insert(5);

    apply(il.begin(), il.end(), add_num);
    apply_arg(il.begin(), il.end(), add_num_arg, 100);
    apply_arg_ref(il.begin(), il.end(), print_ol<int>, cout);

    Ordered_list<int> il1(std::move(il));

    apply_arg_ref(il1.begin(), il1.end(), print_ol<int>, cout);

    Ordered_list<int> il2;
    il2 = std::move(il1);

    for (auto& i : il2) {
        i += 10;
    }

    auto obj = il2.find(110);

    String me("Steve");
    String you("You");
    String abby("Abby");

    Ordered_list<String> strings;

    strings.insert(std::move(me));
    strings.insert(std::move(you));
    strings.insert(std::move(abby));

    apply_arg_ref(strings.begin(), strings.end(), print_ol<String>, cout);


    return 0;
}