#ifndef PERSON_H
#define PERSON_H

#include <ostream>
#include <string>

/* A Person object simply contains Strings for a person's data.
Once created, the data cannot be modified. */

class Person {
public:
    Person(const std::string& firstname_, const std::string& lastname_, const std::string& phoneno_)
        : m_firstname(firstname_), m_lastname(lastname_), m_phoneno(phoneno_) {}
    // construct a Person object with only a lastname
    Person(const std::string& lastname_) : m_lastname(lastname_) {}

    // Construct a Person object from a file stream in save format.
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Person already exists or not.
    // Input for a member variable value is read directly into the member variable.
    Person(std::ifstream& is);

    // Accessors
    std::string get_lastname() const {
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

    std::string m_firstname;
    std::string m_lastname;
    std::string m_phoneno;
};


// output firstname, lastname, phoneno with one separating space, NO endl
std::ostream& operator<< (std::ostream& os, const Person& person);

#endif // PERSON_H
