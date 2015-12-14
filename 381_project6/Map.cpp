#include "Map.h"
#include "Geometry.h"
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>


using std::vector;
using std::string;
using std::cout; using std::endl;
using std::setw;


Map::Map(const string& name, const Point& origin)
    : View(name), m_origin(origin)
{
}

void Map::update_location(const string& name, const Point& location) {
    m_grid_objects[name] = location;
}

void Map::update_remove(const string& name) {
    auto iter = m_grid_objects.find(name);

    // Do nothing if name not found
    if (iter == m_grid_objects.end()) {
        return;
    }

    // Remove found object
    m_grid_objects.erase(iter);
}

Map::Grid_info Map::generate_grid_info() {
    // Holds any objects found to be off the grid
    Grid_info grid_info{ Grid_t(get_size(), vector<vector<char>>(get_size(), vector<char> {'.', ' '})),
                         Offgrid_objs_t() };

    // Add objects to the grid to be drawn
    for (auto& pair : m_grid_objects) {
        // row and column of the cell
        int row, col;
        bool is_on_grid = get_subscripts(col, row, pair.second);

        // If offgrid, add to offgrid container then continue to next object
        if (!is_on_grid) {
            grid_info.m_offgrid_objs.push_back(&pair.first);
            continue;
        }

        // check if cell is empty, empty cell indicated by '.' in first position
        if (grid_info.m_grid[row][col][0] == '.') {
            // cell is empty, add the object
            grid_info.m_grid[row][col][0] = pair.first[0];
            grid_info.m_grid[row][col][1] = pair.first[1];
        }
        else {
            // cell is already occupied, indicate multiple objects in cell
            grid_info.m_grid[row][col][0] = '*';
            grid_info.m_grid[row][col][1] = ' ';
        }
    }

    return grid_info;
}

void Map::print_grid_helper(const Grid_t &grid) {
    // Print doubles with no decimal points
    cout.precision(0);

    // Print Grid
    for (int i = get_size() - 1; i >= 0; i--) {
        // Y-axis labels every 3 lines
        if (i % 3 != 0) {
            cout << "    "; // leading spaces when no axis label
        }
        else {
            cout << setw(4) << m_origin.y + get_scale() * static_cast<double>(i);
        }

        cout << ' '; // One space between label and grid items

        // Print current grid row
        for (int j = 0; j < get_size(); j++) {
            cout << grid[i][j][0] << grid[i][j][1];
        }

        cout << '\n';
    }

    // X-axis labels
    for (int i = 0; i < get_size(); i = i + 3) {
        cout << "  " << setw(4) << m_origin.x + get_scale() * static_cast<double>(i);
    }
    cout << endl;
}

void Map::print_offgrid_helper(const vector<const string*> &offgrid_objects) {
    auto iter = offgrid_objects.begin();

    // First offgrid object (no comma in front)
    cout << **iter;
    iter++;

    // All subsequent offgrid objects (preceded by commas)
    while (iter != offgrid_objects.end()) {
        cout << ", " << *(*iter++);
    }

    cout << " outside the map\n";
}

void Map::clear() {
    m_grid_objects.clear();
}

const Point& Map::get_origin() const {
    return m_origin;
}

void Map::set_origin(const Point& origin) {
    m_origin = origin;
}

// Calculate the cell subscripts corresponding to the supplied location parameter, 
// using the current size, scale, and origin of the display. 
// This function assumes that origin is a  member variable of type Point, 
// scale is a double value, and size is an integer for the number of rows/columns 
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool Map::get_subscripts(int &ix, int &iy, Point location)
{
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - m_origin) / get_scale();
    // truncate coordinates to integer after taking the floor
    // floor function will return the largest integer smaller than the supplied value
    // even for negative values, so -0.05 => -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    // if out of range, return false
    if ((ix < 0) || (ix >= get_size()) || (iy < 0) || (iy >= get_size())) {
        return false;
    }
    else {
        return true;
    }
}
