#include "Model.h"
#include "Structure_factory.h"
#include "Agent_factory.h"
#include "Geometry.h"

/* ***  Here define g_Model_ptr */
Model* g_Model_ptr = nullptr;

/* *** create the initial structures and agents using the following calls to the factories
in the Model constructor. You will have to write code to save the returned pointers. 
Do not change the order of these calls. You should delete this comment. */

Model::Model() {
    create_structure("Rivendale", "Farm", Point(10., 10.));
    create_structure("Sunnybrook", "Farm", Point(0., 30.));
    create_structure("Shire", "Town_Hall", Point(20., 20.));
    create_structure("Paduca", "Town_Hall", Point(30., 30.));

    create_agent("Pippin", "Peasant", Point(5., 10.));
    create_agent("Merry", "Peasant", Point(0., 25.));
    create_agent("Zug", "Soldier", Point(20., 30.));
    create_agent("Bug", "Soldier", Point(15., 20.));
}
