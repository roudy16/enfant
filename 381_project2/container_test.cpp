#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include <iostream>

using namespace std;

int main() {
    Ordered_list<int> il;
    auto it = il.begin();
    cout << &it << endl;

    return 0;
}