#include "View.h"
#include "Geometry.h"
#include "Utility.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// default View settings
const double kDEFAULT_VIEW_SCALE = 2.0;
const double kDEFAULT_VIEW_ORIGINX = -10.0;
const double kDEFAULT_VIEW_ORIGINY = -10.0;
const int kDEFAULT_VIEW_SIZE = 25;

// min, max View sizes (inclusive)
const int kMAX_VIEW_SIZE = 30;
const int kMIN_VIEW_SIZE = 7;

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

View::View() : m_origin(kDEFAULT_VIEW_ORIGINX, kDEFAULT_VIEW_ORIGINY),
               m_scale(kDEFAULT_VIEW_SCALE), m_size(kDEFAULT_VIEW_SIZE) 
{
}

void View::update_location(const std::string& name, Point location) {
    m_grid_objects[name] = location;
}

void View::update_remove(const std::string& name) {
    auto iter = m_grid_objects.find(name);

    // Do nothing if name not found
    if (iter == m_grid_objects.end()) {
        return;
    }

    // Remove found object
    m_grid_objects.erase(iter);
}

void View::print_grid_helper(const Grid_t &grid) {
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

void View::draw() {
    // Save previous output settings
    Cout_settings_saver css;

    // Create an empty grid with dimensions m_size by m_size and each cell holds
    // ". " to indicate it is empty
    Grid_t grid(m_size, vector<vector<char>>(m_size, vector<char>{'.', ' '}));

    // Display doubles with exactly 2 digits after the decimal point
    cout << fixed;
    cout << setprecision(2);

    // Print current View settings
    cout << "Display size: " << m_size << ", scale: " << m_scale << ", origin: " << m_origin << endl;

    // Holds any objects found to be off the grid
    vector<const string*> offgrid_objects;

    // Add objects to the grid to be drawn
    for (auto& pair : m_grid_objects) {
        // row and column of the cell
        int row, col;
        bool is_on_grid = get_subscripts(col, row, pair.second);

        // If offgrid, add to offgrid container then continue to next object
        if (!is_on_grid) {
            offgrid_objects.push_back(&pair.first);
            continue;
        }

        // check if cell is empty, empty cell indicated by '.' in first position
        if (grid[row][col][0] == '.') {
            // cell is empty, add the object
            grid[row][col][0] = pair.first[0];
            grid[row][col][1] = pair.first[1];
        }
        else {
            // cell is already occupied, indicate multiple objects in cell
            grid[row][col][0] = '*';
            grid[row][col][1] = ' ';
        }
    }

    // Print offgrid objects message if any
    if (!offgrid_objects.empty()) {
        print_offgrid_helper(offgrid_objects);
    }

    // Print the objects that are on the grid
    print_grid_helper(grid);

    // Restore previous output settings when function ends
}

void View::clear() {
    m_grid_objects.clear();
}

void View::set_size(int size_) {
    // Check size within range.
    if (size_ > kMAX_VIEW_SIZE) {
        throw Error("New map size is too big!");
    }
    else if (size_ < kMIN_VIEW_SIZE) {
        throw Error("New map size is too small!");
    }

    m_size = size_;
}

void View::set_scale(double scale_) {
    // Ensure scale is positive
    if (scale_ <= 0.0) {
        throw Error("New map scale must be positive!");
    }

    m_scale = scale_;
}

void View::set_origin(Point origin_) {
    m_origin = origin_;
}

void View::set_defaults() {
    m_origin.x = kDEFAULT_VIEW_ORIGINX;
    m_origin.y = kDEFAULT_VIEW_ORIGINY;
    m_scale = kDEFAULT_VIEW_SCALE;
    m_size = kDEFAULT_VIEW_SIZE;
}


// Calculate the cell subscripts corresponding to the supplied location parameter, 
// using the current size, scale, and origin of the display. 
// This function assumes that origin is a  member variable of type Point, 
// scale is a double value, and size is an integer for the number of rows/columns 
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool View::get_subscripts(int &ix, int &iy, Point location)
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
