#ifndef UTILITY_H
#define UTILITY_H

#ifdef _MSC_VER
  #if (_MSC_VER < 1900)
    #define noexcept throw()
    #define constexpr const
  #endif
#endif

#include <string>
#include <exception>
#include <memory>
#include <map>
#include <cassert>

// Forward declarations
class Agent;
class Sim_object;

/* ################################################################## */
/* Utility declarations, functions, and classes used by other modules */
/* ################################################################## */

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
