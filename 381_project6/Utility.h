#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <exception>
#include <memory>
#include <map>
#include <cassert>

// Hack for developing in Visual Studio versions that don't support all C++ features
#ifdef _MSC_VER
  #if (_MSC_VER < 1900)
    #define noexcept throw()
    #define constexpr const
  #endif
#endif

/* ################################################################## */
/* Utility declarations, functions, and classes used by other modules */
/* ################################################################## */

// Returns true if two doubles compare equal within an allowable tolerance.
bool double_tolerance_compare_eq(const double lhs, const double rhs) noexcept;

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
