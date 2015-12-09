#include "World_map.h"
#include "Utility.h"
#include <string>
#include <iostream>

using std::string;
using std::cout; using std::endl;

// default World_map settings
constexpr double kDEFAULT_MAP_SCALE = 2.0;
constexpr double kDEFAULT_MAP_ORIGINX = -10.0;
constexpr double kDEFAULT_MAP_ORIGINY = -10.0;
constexpr int kDEFAULT_MAP_SIZE = 25;
// min, max World_map sizes (inclusive)
constexpr int kMAX_MAP_SIZE = 30;
constexpr int kMIN_MAP_SIZE = 7;


World_map::World_map(const string& name) 
    : Map(name, Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY)),
      m_scale(kDEFAULT_MAP_SCALE), m_size(kDEFAULT_MAP_SIZE)
{
}

void World_map::do_draw_header() {
    // Print current Map settings
    cout << "Display size: " << m_size
         << ", scale: " << m_scale
         << ", origin: " << get_origin() << endl;
}

void World_map::do_draw_body() {
    Grid_info grid_info = generate_grid_info();

    // Print offgrid objects message if any
    if (!grid_info.m_offgrid_objs.empty()) {
        print_offgrid_helper(grid_info.m_offgrid_objs);
    }

    // Print the objects that are on the grid
    print_grid_helper(grid_info.m_grid);
}

void World_map::set_origin(const Point& origin) {
    Map::set_origin(origin);
}

void World_map::set_size(int size_) {
    // Check size within range.
    if (size_ > kMAX_MAP_SIZE) {
        throw Error("New map size is too big!");
    }
    else if (size_ < kMIN_MAP_SIZE) {
        throw Error("New map size is too small!");
    }

    m_size = size_;
}

void World_map::set_scale(double scale_) {
    // Ensure scale is positive
    if (scale_ <= 0.0) {
        throw Error("New map scale must be positive!");
    }

    m_scale = scale_;
}

void World_map::set_defaults() {
    set_origin(Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY));
    m_scale = kDEFAULT_MAP_SCALE;
    m_size = kDEFAULT_MAP_SIZE;
}

double World_map::get_scale() const {
    return m_scale;
}

int World_map::get_size() const {
    return m_size;
}

