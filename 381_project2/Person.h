#ifndef PERSON_H
#define PERSON_H

#include "String.h"

/* A Person object simply contains Strings for a person's data.
Once created, the data cannot be modified. */

/* *** NOTE: If after a function header is a comment "fill this in" remove the comment and replace
it with the proper code here in the header file.  All other functions should be defined
in the .cpp file. 
Comments starting with "***" are instructors to you - remove them from your finished code.
Remove this comment too. */

class Person {
public:
    Person(const String& firstname_, const String& lastname_, const String& phoneno_)
        : m_firstname(firstname_), m_lastname(lastname_), m_phoneno(phoneno_) {}
    // construct a Person object with only a lastname
    Person(const String& lastname_) : m_lastname(lastname_) {}

    // Construct a Person object from a file stream in save format.
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Person already exists or not.
    // Input for a member variable value is read directly into the member variable.
    Person(std::ifstream& is);

    // Accessors
    String get_lastname() const {
        return m_lastname;
    }

    // Write a Person's data to a stream in save format with final endl.
    void save(std::ostream& os) const;

    // This operator defines the order relation between Persons, based just on the last name
    bool operator< (const Person& rhs) const {
        return m_lastname < rhs.m_lastname;
    }

    friend std::ostream& operator<< (std::ostream&, const Person&);

private:
    Person(const Person& person) = delete;
    Person(Person&& person) = delete;
    Person& operator=(const Person& rhs) = delete;

    String m_firstname;
    String m_lastname;
    String m_phoneno;
};


// output firstname, lastname, phoneno with one separating space, NO endl
std::ostream& operator<< (std::ostream& os, const Person& person);

#endif // PERSON_H
