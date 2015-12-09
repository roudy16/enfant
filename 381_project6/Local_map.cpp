#include "Map.h"
#include "Local_map.h"
#include <string>
#include <iostream>

using std::string;
using std::cout; using std::endl;



// default Local_map settings
constexpr double kDEFAULT_LOCALMAP_SCALE = 2.0;
constexpr int kDEFAULT_LOCALMAP_SIZE = 9;

Local_map::Local_map(const string& name) 
    : Map(name, Point())
{
}

void Local_map::do_draw_header() {
    cout << "Local view for: " << get_name() << endl;
}

void Local_map::do_draw_body() {
    Grid_info grid_info = generate_grid_info();

    // Print the objects that are on the grid
    print_grid_helper(grid_info.m_grid);
}

void Local_map::update_location(const string& name, Point location) {
    Map::update_location(name, location);

    // if the name of the updated object is the same as the focus of this
    // map then update this map's origin to the focus's new location plus
    // the displacement from the origin to the center of the map
    if (get_name() == name) {
        constexpr double displacement_amount = -(kDEFAULT_LOCALMAP_SIZE / 2.0) *
                                                 kDEFAULT_LOCALMAP_SCALE;
        const Cartesian_vector displacement(displacement_amount, displacement_amount);
        set_origin(location + displacement);
    }
}

double Local_map::get_scale() const {
    return kDEFAULT_LOCALMAP_SCALE;
}

int Local_map::get_size() const {
    return kDEFAULT_LOCALMAP_SIZE;
}
