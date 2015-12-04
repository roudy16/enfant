#ifndef UTILITY_H
#define UTILITY_H

#include "Geometry.h"
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

class Sim_object;

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

class Closest_to_obj {
public:
    Closest_to_obj(std::shared_ptr<Sim_object>& obj_ptr);

    template <typename T>
    bool operator()(T& lhs, T& rhs) {
        if (lhs.second->get_name() == m_name) {
            return false;
        }
        if (rhs.second->get_name() == m_name) {
            return true;
        }

        double dist_to_lhs = cartesian_distance(m_location, lhs.second->get_location());
        double dist_to_rhs = cartesian_distance(m_location, rhs.second->get_location());

        if (dist_to_lhs == dist_to_rhs) {
            return lhs.second->get_name() < rhs.second->get_name();
        }

        return dist_to_lhs < dist_to_rhs;
    }

private:
    Point m_location;
    std::string m_name;
};

#endif // UTILITY_H
