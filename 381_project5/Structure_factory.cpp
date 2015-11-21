#include "Structure_factory.h"
#include "Structure.h"
#include "Town_Hall.h"
#include "Farm.h"
#include "Geometry.h"
#include "Utility.h"

Structure * create_structure(const std::string& name, const std::string& type, Point location) {
    Structure* new_structure_ptr = nullptr;

    // Determine what type of Structure to create, throw Error if no such type
    if (type == "Farm") {
        new_structure_ptr = new Farm(name, location);
    }
    else if (type == "Town_Hall") {
        new_structure_ptr = new Town_Hall(name, location);
    }
    else {
        throw Error("Trying to create structure of unknown type!");
    }

    return new_structure_ptr;
}
