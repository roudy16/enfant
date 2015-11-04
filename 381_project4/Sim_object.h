#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H

/* The Sim_object class provides the interface for all of simulation objects. 
It also stores the object's name, and has pure virtual accessor functions for 
the object's position and other information. */

/* 
*** This skeleton file shows the required public interface for the class, which you may not modify. 
If no protected members are shown, there must be none in your version. 
If any protected or private members are shown here, then your class must also have them and use them as intended.
You must delete this comment and all other comments that start with "***".
*/

class Sim_object {
public:
    Sim_object(const std::string& name_);
    // *** supply the appropriate destructor declaration
    
    const std::string& get_name() const
        {return name;}
            
    /*** Interface for derived classes ***/
    // *** declare the following to be pure virtual functions
    // ask model to notify views of current state
    virtual void broadcast_current_state() const
    Point get_location() const
    void describe() const
    void update()

private:
    std::string name;
};

#endif // SIM_OBJECT_H
