#ifndef MAP_H
#define MAP_H

#include "View.h"
#include "Geometry.h"
#include <string>
#include <map>
#include <vector>

// Map is an abstract base class for Views that draw a visual representation 
// of some area of the world
class Map : public View {
public:
    // Save the supplied name and location for future use in a draw() call
    // If the name is already present,the new location replaces the previous one.
    void update_location(const std::string& name, const Point& location) override;

    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;

    // Discard the saved information - drawing will show only a empty pattern
    void clear() override;

protected:
    using Grid_t = std::vector<std::vector<std::vector<char>>>;
    using Offgrid_objs_t = std::vector<const std::string*>;

    struct Grid_info {
        Grid_t         m_grid;
        Offgrid_objs_t m_offgrid_objs;
    };

    // Constructor sets the name and default size, scale, and origin
    Map(const std::string& name, const Point& origin);

    // Populates a grid that represents an area of the world with representations
    // of objects and stores a list of objects that lie outside grid
    Grid_info generate_grid_info();
    // Prints the grid along with axis label info
    void print_grid_helper(const Grid_t &grid);
    // Prints objects that are not visible on the grid, offgrid objects must be in
    // passed in container before function is called
    void print_offgrid_helper(const Offgrid_objs_t &objs);

    // returns reference to Map's origin Point
    const Point& get_origin() const;

    // any values are legal for the origin
    virtual void set_origin(const Point& origin);

    // Hooks used to get the size and scale of a Map derived class
    virtual double get_scale() const = 0;
    virtual int get_size() const = 0;

private:
    using Map_objects_t = std::map<std::string, Point>;

    bool get_subscripts(int &ix, int &iy, Point location);

    Point         m_origin;
    Map_objects_t m_grid_objects;
};

#endif // MAP_H
