#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "Map.h"
#include "Geometry.h"
#include <string>


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

#endif // WORLD_MAP_H
