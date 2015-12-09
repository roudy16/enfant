#include "Structure_factory.h"
#include "Structure.h"
#include "Town_Hall.h"
#include "Farm.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <memory>

using std::string;
using std::shared_ptr; using std::static_pointer_cast; using std::make_shared;

shared_ptr<Structure> create_structure(const string& name, const string& type, Point location) {
    shared_ptr<Structure> new_structure_ptr;

    // Determine what type of Structure to create, throw Error if no such type
    if (type == "Farm") {
        new_structure_ptr = static_pointer_cast<Structure>(make_shared<Farm>(name, location));
    }
    else if (type == "Town_Hall") {
        new_structure_ptr = static_pointer_cast<Structure>(make_shared<Town_Hall>(name, location));
    }
    else {
        throw Error("Trying to create structure of unknown type!");
    }

    return new_structure_ptr;
}
