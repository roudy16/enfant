#ifndef VIEWS_H
#define VIEWS_H

#include "Geometry.h"
#include "View.h"
#include <vector>
#include <string>
#include <map>

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


// Map is an abstract base class for Views that draw a visual representation 
// of some area of the world
class Map : public View {
public:
    // Save the supplied name and location for future use in a draw() call
    // If the name is already present,the new location replaces the previous one.
    void update_location(const std::string& name, Point location) override;

    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;

    // Discard the saved information - drawing will show only a empty pattern
    void clear() override;

protected:
    using Grid_t = std::vector<std::vector<std::vector<char>>> ;
    using Offgrid_objs_t = std::vector<const std::string*>;

    struct Grid_info {
        Grid_t m_grid;
        Offgrid_objs_t m_offgrid_objs;
    };

    // Constructor sets the name and default size, scale, and origin
    Map(const std::string& name, const Point& origin);

    // returns reference to Map's origin Point
    const Point& get_origin() const;

    // any values are legal for the origin
    void set_origin(const Point& origin);

    virtual double get_scale() const = 0;

    virtual int get_size() const = 0;

    // Populates a grid that represents an area of the world with representations
    // of objects 
    Grid_info generate_grid_info();

    void print_grid_helper(const Grid_t &grid);

private:
    using Objects_t = std::map<std::string, Point>;

    bool get_subscripts(int &ix, int &iy, Point location);

    Point m_origin;
    Objects_t m_grid_objects;
};

class World_map : public Map {
public:
    World_map(const std::string& name);
    virtual ~World_map();

    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_);

    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);

    // set the parameters to the default values
    void set_defaults();

    void draw() override;

    World_map() = delete;
    World_map(const World_map&) = delete;
    World_map& operator= (const World_map&) = delete;
    World_map(World_map&&) = delete;
    World_map& operator= (World_map&&) = delete;

private:
    double get_scale() const override;
    int get_size() const override;

    double m_scale;
    int m_size;
};

class Local_map : public Map {
public:
    Local_map(const std::string& name);
    virtual ~Local_map();

    void update_location(const std::string& name, Point location) override;
    void draw() override;

    Local_map() = delete;
    Local_map(const Local_map&) = delete;
    Local_map& operator= (const Local_map&) = delete;
    Local_map(Local_map&&) = delete;
    Local_map& operator= (Local_map&&) = delete;

private:
    double get_scale() const override;
    int get_size() const override;
};

#endif // VIEWS_H
