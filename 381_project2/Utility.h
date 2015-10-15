#ifndef UTILITY_H
#define UTILITY_H

class Person;

/* Utility functions, constants, and classes used by more than one other modules */


// a simple class for error exceptions - msg points to a C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
		{}
	const char* msg;
};

bool person_to_String_comp(String lastname, const Person *const person_ptr);

#endif // UTILITY_H
