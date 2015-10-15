#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include "Person.h"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
    Person me("Steve", "Roudy", "517");
    ofstream ofs("test_person_save.txt");
    me.save(ofs);
    ifstream ifs("test_person_save.txt");
    Person me_too(ifs);

    Person not_me("Roody");

    cout << (not_me < me_too);

    return 0;
}
