#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include <string>
#include <map>

// Forward declaration
struct Point;

/* *** View class ***
The View class encapsulates the data and functions needed to generate the map
display, and control its properties. It has a "memory" for the names and locations
of the to-be-plotted objects.

Usage: 
1. Call the update_location function with the name and position of each object
to be plotted. If the object is not already in the View's memory, it will be added
along with its location. If it is already present, its location will be set to the 
supplied location. If a single object changes location, its location can be separately
updated with a call to update_location. 
2. Call the update_remove function with the name of any object that should no longer
be plotted. This must be done *after* any call to update_location that 
has the same object name since update_location will add any object name supplied.
3. Call the draw function to print out the map. 
4. As needed, change the origin, scale, or displayed size of the map 
with the appropriate functions. Since the view "remembers" the previously updated
information, immediately calling the draw function will print out a map showing the previous objects
using the new settings.
*/


class View {
public:
    // Constructor sets the name of the View
    View(const std::string& name);

    virtual ~View();

    // Save the supplied name and location for future use in a draw() call
    // If the name is already present,the new location replaces the previous one.
    virtual void update_location(const std::string& name, const Point& location);
    virtual void update_health(const std::string& name, double health);
    virtual void update_amount(const std::string& name, double amount);
    virtual void update_remove(const std::string& name);
    virtual void clear();

    // prints out the View information
    void draw();

    // returns the name of the View
    const std::string& get_name();

protected:
    // hooks called when 'draw' is called, to be defined in derived classes.
    // draw_header() does nothing in this base class
    virtual void do_draw_header();
    virtual void do_draw_body() = 0;

private:
    std::string m_name;
};

#endif // VIEW_H
