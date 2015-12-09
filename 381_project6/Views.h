#ifndef VIEWS_H
#define VIEWS_H

#include "Geometry.h"
#include "View.h"
#include <vector>
#include <string>
#include <map>

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

/*
    A World_map shows a large area of the world. It keeps track of all objects that
    have reported a location and displays information about the whereabouts of 
    those objects. Objects outside the visible area of the World_map will be
    reported as such.
*/
class World_map : public Map {
public:
    World_map(const std::string& name);

    // any values are legal for the origin
    void set_origin(const Point& origin) override;

    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_);

    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);

    // set the parameters to the default values
    void set_defaults();

    World_map() = delete;
    World_map(const World_map&) = delete;
    World_map& operator= (const World_map&) = delete;
    World_map(World_map&&) = delete;
    World_map& operator= (World_map&&) = delete;

private:
    void do_draw_header() override;
    void do_draw_body() override;

    double get_scale() const override;
    int get_size() const override;

    double m_scale;
    int    m_size;
};

/*
    A Local_map shows the immediate area around an object. As that focus object moves
    so to does the Local_map. The Local_map is similar to the World_map in its display
    but does not report objects that are outside its visible area.
*/
class Local_map : public Map {
public:
    Local_map(const std::string& name);

    void update_location(const std::string& name, Point location) override;

    Local_map() = delete;
    Local_map(const Local_map&) = delete;
    Local_map& operator= (const Local_map&) = delete;
    Local_map(Local_map&&) = delete;
    Local_map& operator= (Local_map&&) = delete;

private:
    // Hooks for drawing
    void do_draw_header() override;
    void do_draw_body() override;

    double get_scale() const override;
    int get_size() const override;
};

/*
    Status is an abstract base class of Views that can display numerical information
    related to an object.
*/
class Status : public View {
public:
    // Make Status an abstract class
    virtual ~Status() = 0;

    // Remove named object from Status objects container
    void update_remove(const std::string& name) override;
    // Remove all objects
    void clear() override;

protected:
    Status(const std::string& name);

    // Updates an objects status value, adds object name to container if the
    // object is not already there.
    void update_status(const std::string& name, double val);

private:
    using Status_objects_t = std::map<std::string, double>;

    // Hook for drawing
    void do_draw_body() override;

    Status_objects_t m_objects;
};

class Health_status : public Status {
public:
    explicit Health_status();

    // Updates an objects health value, adds object name to container if the
    // object is not already there.
    void update_health(const std::string& name, double health) override;

    Health_status(const Health_status&) = delete;
    Health_status& operator= (const Health_status&) = delete;
    Health_status(Health_status&&) = delete;
    Health_status& operator= (Health_status&&) = delete;

private:
    // Hook for drawing
    void do_draw_header() override;
};

class Amount_status : public Status {
public:
    explicit Amount_status();

    // Updates an objects amount value, adds object name to container if the
    // object is not already there.
    void update_amount(const std::string& name, double amount) override;

    Amount_status(const Amount_status&) = delete;
    Amount_status& operator= (const Amount_status&) = delete;
    Amount_status(Amount_status&&) = delete;
    Amount_status& operator= (Amount_status&&) = delete;

private:
    //Hook for drawing
    void do_draw_header() override;
};

#endif // VIEWS_H
