#ifndef PERSON_H
#define PERSON_H

#include <ostream>
#include <string>
#include <list>

// Forward declare Meeting class
class Meeting;

/* A Person object simply contains Strings for a person's data.
Once created, the data cannot be modified. */

class Person {
public:
    // Destructor
    ~Person() = default;

    // construct a Person object with first and last name and phoneno
    Person(const std::string& firstname_, const std::string& lastname_, const std::string& phoneno_);
    // construct a Person object with only a lastname
    Person(const std::string& lastname_);

    // Construct a Person object from a file stream in save format.
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Person already exists or not.
    // Input for a member variable value is read directly into the member variable.
    Person(std::ifstream& is);

    // Accessors
    std::string get_lastname() const;

    // Write a Person's data to a stream in save format with final endl.
    void save(std::ostream& os) const;

    // Returns true if Person has commitment at time that is conflicts with meeting
    bool has_commitment_conflict(const Meeting* const meeting_ptr, int time) const;

    // Returns true if Person has any commitments
    bool has_commitments() const;

    // Adds a new commitment to commitments list, throws an Error if Person
    // already has a commitment at that time
    void add_commitment(const Meeting*) const;

    // Removes a commitment from the commitments list
    void remove_commitment(int time) const;

    // Changes the commitment at the old time to a new commitment in a new
    // Room at a new time
    void change_commitment(int old_time, const Meeting* new_meeting_ptr) const;

    // Prints all commitments to std::cout
    void print_commitments() const;

    // This operator defines the order relation between Persons, based just on the last name
    bool operator< (const Person& rhs) const;

    friend std::ostream& operator<< (std::ostream&, const Person&);

private:
    // Default Constructor
    Person() = delete;
    // Move Constructor
    Person(Person&&) = delete;
    // Copy Constructor
    Person(const Person&) = delete;
    // Move Assignment
    Person& operator=(Person&&) = delete;
    // Copy Assignment
    Person& operator=(Person&) = delete;

    // Private struct that only the Person needs to have access to
    struct Commitment {
        Commitment(const Meeting*);

        // Print commitment info to cout
        void print() const;
        // Commitments are ordered first by room number then by time
        // of the associated Meeting
        bool operator< (const Commitment& rhs) const;

        // Pointer to Commitment's associated Meeting
        const Meeting* mp_meeting;
    };

    using Commitments_t = std::list<Commitment>;

    // Returns an iterator to the Commitment with matching time if it exists,
    // returns an iterator to end otherwise.
    Commitments_t::iterator find_commitment(int time) const;

    // Checks list ordering invarient, returns true if invarient holds.
    // NOTE: This function only performs checks if DEBUG preprocessor symbol is defined
    // otherwise it always returns true
    bool verify_commitments_ordering() const;

    // The Person should not change name or phone number but over its lifetime
    // its commitments can change
    mutable Commitments_t m_commitments;
    std::string m_firstname;
    std::string m_lastname;
    std::string m_phoneno;
};

// output firstname, lastname, phoneno with one separating space, NO endl
std::ostream& operator<< (std::ostream& os, const Person& person);

#endif // PERSON_H
