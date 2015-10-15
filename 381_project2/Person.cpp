#include "Person.h"
#include "String.h"
#include <fstream>
#include <ostream>

using std::endl;

Person::Person(std::ifstream& is){
    is >> m_firstname;
    is >> m_lastname;
    is >> m_phoneno;
}

void Person::save(std::ostream& os) const {
    os << *this << endl;
}

std::ostream& operator<< (std::ostream& os, const Person& person) {
    os << person.m_firstname << ' ' << person.m_lastname << ' ' << person.m_phoneno;
    return os;
}
