#ifndef PERSON_H
#define PERSON_H

#include <ostream>
#include <string>
#include <list>


/* A Person object simply contains Strings for a person's data.
Once created, the data cannot be modified. */

class Person {
public:
    Person();
    ~Person();

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

    // TODO fuck, there is no good way to do this... seems like you have to break encapsulation
    // Write a Person's data to a stream in save format with final endl.
    void save(std::ostream& os) const;

    bool has_commitment(const int time) const;

    void add_commitment(const int new_room_number, const int new_time);

    void change_commitment(const int old_time, const int new_room_number, const int new_time);

    void print_commitments() const;



    // This operator defines the order relation between Persons, based just on the last name
    bool operator< (const Person& rhs) const {
        return m_lastname < rhs.m_lastname;
    }

    friend std::ostream& operator<< (std::ostream&, const Person&);

private:
    class Commitment {
    public:
        int get_room_number() const;
        int get_time() const;
        void change_to(const int new_room_number, const int new_time);
        bool operator< (const Commitment& rhs) const;

    private:
        int mp_room_number;
        int m_time;
    };


    Person(const Person& person) = delete;
    Person(Person&& person) = delete;
    Person& operator=(const Person& rhs) = delete;

    std::list<Commitment> m_commitments;
    std::string m_firstname;
    std::string m_lastname;
    std::string m_phoneno;

};


// output firstname, lastname, phoneno with one separating space, NO endl
std::ostream& operator<< (std::ostream& os, const Person& person);

#endif // PERSON_H
