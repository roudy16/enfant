#ifndef LOCAL_MAP_H
#define LOCAL_MAP_H

#include "Map.h"
#include "Geometry.h"
#include <string>

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

#endif // LOCAL_MAP_H
