#include "View_factory.h"
#include "Sim_object.h"
#include "View.h"
#include "World_map.h"
#include "Local_map.h"
#include "Health_status.h"
#include "Amount_status.h"
#include "Utility.h"
#include "Model.h"
#include <memory>
#include <string>

using std::string;
using std::shared_ptr; using std::make_shared;

View_factory_return create_view(const string& name) {
    View_factory_return ret_val;

    // Check what type of View user wants
    if (name == "map") {
        // create a World map that shows a large area of the world
        shared_ptr<World_map> world_map_ptr = make_shared<World_map>("map");
        ret_val.view_ptr = world_map_ptr;
        ret_val.world_map_ptr = world_map_ptr;
    }
    else if (name == "health") {
        // create a health status view that shows the health of Agents
        ret_val.view_ptr = make_shared<Health_status>();
    }
    else if (name == "amounts") {
        // create an amount status view that shows the food amounts of Sim_objects
        ret_val.view_ptr = make_shared<Amount_status>();
    }
    else {
        // Create a local map view centered on a Sim_object that matches the input name,
        // throw an Error if no such object exists
        shared_ptr<Sim_object> obj_ptr = Model::get_instance()->get_obj_ptr(name);

        if (!obj_ptr) {
            throw Error("No object of that name!");
        }

        ret_val.view_ptr = make_shared<Local_map>(obj_ptr->get_name());
    }

    return ret_val;
}
