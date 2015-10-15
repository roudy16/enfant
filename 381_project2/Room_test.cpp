#include "Ordered_list.h"
#include "String.h"
#include "Utility.h"
#include "Person.h"
#include "Meeting.h"
#include "Room.h"
#include <iostream>
#include <fstream>
#include <utility>

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
    Person Ronda("Ronda", "Rousey", "1-800-PUNCHME");
    Person Me("Steve", "Roudy", "312");
    Person Dale("Dale", "Junior", "55");
    Person Marv("Marv", "Burnsie", "1234");
    Person James("Jamse", "Bond", "007");
    Person Austin("Austin", "Powers", "69_Baby,_Yeah!");

    Ordered_list<const Person*, Less_than_ptr<const Person*>> people;
    people.insert(&Ronda);
    people.insert(&Me);
    people.insert(&Dale);
    people.insert(&Marv);
    people.insert(&James);
    people.insert(&Austin);

    Room r0(1000);
    Room r1(1001);
    Room r2(1002);
    Room r3(1003);

    Ordered_list<Room* const, Less_than_ptr<Room* const>> rooms;
    rooms.insert(&r1);
    rooms.insert(&r3);
    rooms.insert(&r0);
    rooms.insert(&r2);

    Meeting m0(3, "Pokemon");
    Meeting m1(5, "Judo");
    Meeting m2(10, "Boats");
    Meeting m3(12, "Cars");
    Meeting m4(9, "Country");
    Meeting m5(1, "Women");

    m0.add_participant(&Ronda);
    m0.add_participant(&Me);
    m0.add_participant(&Marv);
    m1.add_participant(&Dale);
    m1.add_participant(&Austin);
    m1.add_participant(&Ronda);
    m2.add_participant(&Dale);
    m2.add_participant(&Austin);
    m2.add_participant(&Ronda);
    m1.remove_participant(&Dale);

    r0.add_Meeting(move(m0));
    r0.add_Meeting(move(m1));
    r0.add_Meeting(move(m2));

    cout << "R0 has meetings: " << r0.has_Meetings() << endl;
    cout << "R1 has meetings: " << r1.has_Meetings() << endl;

    ofstream ofs("test_room_save.txt");
    r0.save(ofs);

    cout << "R0 before clear() ##############" << endl;
    cout << r0 << endl;

    r0.clear_Meetings();
    cout << "R0 after clear() ###############" << endl;
    cout << r0 << endl;

    ifstream ifs("test_room_save.txt");
    Room r4(ifs, people);

    rooms.insert(&r4);

    Room r5 = move(r4);
    cout << "Testing load room from file" << endl;
    cout << r4 << endl;

    cout << r5 << endl;

    return 0;
}
