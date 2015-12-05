#include "Views.h"
#include "Geometry.h"
#include "Utility.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>

using std::vector;
using std::string;
using std::cout; using std::endl;
using std::setw;

// default World_map settings
constexpr double kDEFAULT_MAP_SCALE = 2.0;
constexpr double kDEFAULT_MAP_ORIGINX = -10.0;
constexpr double kDEFAULT_MAP_ORIGINY = -10.0;
constexpr int kDEFAULT_MAP_SIZE = 25;

// min, max World_map sizes (inclusive)
constexpr int kMAX_MAP_SIZE = 30;
constexpr int kMIN_MAP_SIZE = 7;

// default Local_map settings
constexpr double kDEFAULT_LOCALMAP_SCALE = 2.0;
constexpr int kDEFAULT_LOCALMAP_SIZE = 9;


// Prints objects that are not visible on the grid, offgrid objects must be in
// passed in container before function is called
static void print_offgrid_helper(const vector<const string*>& offgrid_objects);

Map::Map(const string& name, const Point& origin)
    : View(name), m_origin(origin)
{
}

void Map::update_location(const string& name, Point location) {
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

World_map::World_map(const string& name) 
    : Map(name, Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY)),
      m_scale(kDEFAULT_MAP_SCALE), m_size(kDEFAULT_MAP_SIZE)
{
}

World_map::~World_map()
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

Local_map::Local_map(const string& name) 
    : Map(name, Point(kDEFAULT_MAP_ORIGINX, kDEFAULT_MAP_ORIGINY))
{
}

Local_map::~Local_map()
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

Status::Status(const string& name) : View(name)
{
}

Status::~Status()
{
}

// if it exists, remove the object that has name from this Status 
void Status::update_remove(const string& name) {
    auto iter = m_objects.find(name);

    // Do nothing if name not found
    if (iter == m_objects.end()) {
        return;
    }

    // Remove found object
    m_objects.erase(iter);
}

void Status::clear() {
    m_objects.clear();
}

// Print object names followed by their associated status value
void Status::do_draw_body() {
    for (auto& obj : m_objects) {
        cout << obj.first << ": " << obj.second << endl;
    }
    cout << "--------------" << endl;
}

// update the status value of an object, if that object is not currently
// contained in this Status then add it.
void Status::update_status(const string& name, double val) {
    auto obj_iter = m_objects.find(name);

    if (obj_iter == m_objects.end()) {
        m_objects[name] = val;
    }
    else {
        obj_iter->second = val;
    }
}

Health_status::Health_status() : Status("health") 
{
}

void Health_status::update_health(const string& name, double health) {
    update_status(name, health);
}

void Health_status::do_draw_header() {
    cout << "Current Health:\n--------------" << endl;
}

Amount_status::Amount_status() : Status("amounts")
{
}

void Amount_status::update_amount(const string& name, double amount) {
    update_status(name, amount);
}

void Amount_status::do_draw_header() {
    cout << "Current Amounts:\n--------------" << endl;
}

