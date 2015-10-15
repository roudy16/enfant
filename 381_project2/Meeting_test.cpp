#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include "Person.h"
#include "Meeting.h"
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
    Person you("Mick", "Jagger", "69");

    Ordered_list<const Person*, Less_than_ptr<const Person*>> people;
    people.insert(&me);
    people.insert(&you);

    Meeting m0(9, "Sun");
    m0.add_participant(&you);
    m0.add_participant(&me);

    ofstream ofs("test_meeting_save.txt");
    m0.save(ofs);

    ifstream ifs("test_meeting_save.txt");
    Meeting m1(ifs, people);
    m1.remove_participant(&you);

    cout << m1;

    return 0;
}