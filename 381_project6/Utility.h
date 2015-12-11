#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <exception>
#include <memory>
#include <map>

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

// Tolerance used when comparing Points
constexpr double kPOINT_TOLERANCE = 1.0e-6;

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


// add a new agent; assumes none with the same name
template <typename C, typename T = C::element_type>
void add_obj_to_map(std::map<C>& container, C obj_ptr) {
    // Add Agent to Sim_objects container as a shared_ptr<Sim_object>
    container[obj_ptr->get_name()] = static_pointer_cast<Sim_object>(obj_ptr);

    // Add Agent to Structures container as an shared_ptr<Agent>
    container[obj_ptr->get_name()] = obj_ptr;
    obj_ptr->broadcast_current_state();
}

// remove Agent from all containers, agent_ptr must not be nullptr
template <typename C, typename T = C::element_type>
void remove_agent(std::map<C>& container, C agent_ptr);
// will throw Error("Agent not found!") if no agent of that name

#endif // UTILITY_H
