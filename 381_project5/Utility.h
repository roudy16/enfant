#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <exception>
#include <memory>

// Hack for developing in Visual Studio versions that don't support all C++ features
#ifdef _MSC_VER
    #if (_MSC_VER < 1900)
        #define noexcept 
        #define constexpr 
    #endif
#endif

// TODO remove. Causes messages to be printed
//#define PRINT_CTORS_DTORS

/* Utility declarations, functions, and classes used by other modules */

// a simple class for error exceptions that inherits from std::exception
class Error : public std::exception {
public:
    Error(const std::string& in_msg = "") :
        msg(in_msg)
        {}
    const char* what() const noexcept override {return msg.c_str();}
private:
    const std::string msg;
};

#endif // UTILITY_H
