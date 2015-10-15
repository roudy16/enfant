#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include "Person.h"
#include <iostream>
#include <fstream>

using namespace std;

template <typename T>
void print_ol_ref(const T& data, ostream& os){
    os << data << endl;
}

template <typename T>
void print_ol_p(const T* const data, ostream& os){
    os << *data << endl;
}

int main() {
    Person me("Steve", "Roudy", "517");
    ofstream ofs("test_person_save.txt");
    me.save(ofs);
    ifstream ifs("test_person_save.txt");
    Person me_too(ifs);

    Person not_me("Roody");

    cout << (not_me < me_too);

    Ordered_list<Person*, Less_than_ptr<Person*>> ol;
    ol.insert(&me);
    ol.insert(&me_too);
    ol.insert(&not_me);
    ol.erase(ol.find(&me_too));
    ol.erase(ol.find(&not_me));

    apply_arg_ref(ol.begin(), ol.end(), print_ol_p<Person>, cout);

    return 0;
}
