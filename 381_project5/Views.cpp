#include "Views.h"
#include "Geometry.h"
#include "Utility.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// default World_map settings
const double kDEFAULT_MAP_SCALE = 2.0;
const double kDEFAULT_MAP_ORIGINX = -10.0;
const double kDEFAULT_MAP_ORIGINY = -10.0;
const int kDEFAULT_MAP_SIZE = 25;

// min, max World_map sizes (inclusive)
const int kMAX_MAP_SIZE = 30;
const int kMIN_MAP_SIZE = 7;

// default Local_map settings
const double kDEFAULT_LOCALMAP_SCALE = 2.0;
const int kDEFAULT_LOCALMAP_SIZE = 9;


// Prints objects that are not visible on the grid, offgrid objects must be in
// passed in container before function is called
static void print_offgrid_helper(const vector<const string*>& offgrid_objects);

namespace {
    // Object to save and restore cout settings
    // Settings saved when object is created then restored when object destroyed
    class Cout_settings_saver {
    public:
        // Save settings
        Cout_settings_saver() : m_form_flags(cout.flags()), m_old_precision(cout.precision())
        {}

        // Restore settings
        ~Cout_settings_saver() {
            cout.flags(m_form_flags);
            cout.precision(m_old_precision);
        }

    private:
        ios::fmtflags m_form_flags;
        streamsize m_old_precision;
    };
}

Map::Map(const std::string& name, const Point& origin, double scale, int size)
    : View(name), m_origin(origin), m_scale(scale), m_size(size) 
{
}

void Map::update_location(const std::string& name, Point location) {
    m_grid_objects[name] = location;
}

void Map::update_remove(const std::string& name) {
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
    Grid_info grid_info{ Grid_t(m_size, vector<vector<char>>(m_size, vector<char> {'.', ' '})),
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
    for (int i = m_size - 1; i >= 0; i--) {
        // Y-axis labels every 3 lines
        if (i % 3 != 0) {
            cout << "    "; // leading spaces when no axis label
        }
        else {
            cout << setw(4) << m_origin.y + m_scale * static_cast<double>(i);
        }

        cout << ' '; // One space between label and grid items

        // Print current grid row
        for (int j = 0; j < m_size; j++) {
            cout << grid[i][j][0] << grid[i][j][1];
        }

        cout << '\n';
    }

    // X-axis labels
    for (int i = 0; i < m_size; i = i + 3) {
        cout << "  " << setw(4) << m_origin.x + m_scale * static_cast<double>(i);
    }
    cout << endl;
}

static void print_offgrid_helper(const vector<const string*> &offgrid_objects) {
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


// Calculate the cell subscripts corresponding to the supplied location parameter, 
// using the current size, scale, and origin of the display. 
// This function assumes that origin is a  member variable of type Point, 
// scale is a double value, and size is an integer for the number of rows/columns 
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool Map::get_subscripts(int &ix, int &iy, Point location)
{
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - m_origin) / m_scale;
    // truncate coordinates to integer after taking the floor
    // floor function will return the largest integer smaller than the supplied value
    // even for negative values, so -0.05 => -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    // if out of range, return false
    if ((ix < 0) || (ix >= m_size) || (iy < 0) || (iy >= m_size)) {
        return false;
    }
    else {
        return true;
    }
}

World_map::World_map(const string& name) 
    : Map(name, Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY),
          kDEFAULT_MAP_SCALE, kDEFAULT_MAP_SIZE)
{
}

void World_map::draw() {
    // Save previous output settings
    Cout_settings_saver css;

    // Display doubles with exactly 2 digits after the decimal point
    cout << fixed;
    cout << setprecision(2);

    // Print current Map settings
    cout << "Display size: " << m_size << ", scale: " << m_scale << ", origin: " << m_origin << endl;

    Grid_info grid_info = generate_grid_info();

    // Print offgrid objects message if any
    if (!grid_info.m_offgrid_objs.empty()) {
        print_offgrid_helper(grid_info.m_offgrid_objs);
    }

    // Print the objects that are on the grid
    print_grid_helper(grid_info.m_grid);

    // Restore previous output settings when function ends
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

void World_map::set_origin(Point origin_) {
    m_origin = origin_;
}

void World_map::set_defaults() {
    m_origin.x = kDEFAULT_MAP_ORIGINX;
    m_origin.y = kDEFAULT_MAP_ORIGINY;
    m_scale = kDEFAULT_MAP_SCALE;
    m_size = kDEFAULT_MAP_SIZE;
}

Local_map::Local_map(const string& name) 
    : Map(name, Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY),
          kDEFAULT_MAP_SCALE, kDEFAULT_MAP_SIZE)
{
}

void Local_map::draw() {
    // Save previous output settings
    Cout_settings_saver css;

    // Display doubles with exactly 2 digits after the decimal point
    cout << fixed;
    cout << setprecision(2);

    // Print current Map settings
    cout << "Display size: " << m_size << ", scale: " << m_scale << ", origin: " << m_origin << endl;

    Grid_info grid_info = generate_grid_info();

    // Print the objects that are on the grid
    print_grid_helper(grid_info.m_grid);

    // Restore previous output settings when function ends
}

void Local_map::update_location(const string& name, Point location) {
    Map::update_location(name, location);

    // if the name of the updated object is the same as the focus of this
    // map then update this map's origin to the focus's new location
    if (get_name() == name) {
        m_origin = location;
    }
}
