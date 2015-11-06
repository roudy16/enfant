#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <exception>

// Hack for developing in Visual Studio versions that don't support noexcept
#ifdef _MSC_VER
    #if (_MSC_VER < 1900)
    #define noexcept 
    #endif
#endif

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

/* *** add any additional declarations used by more than one of your modules here. 
Do not include anything used by only one module.
You must delete this comment. */

#endif

