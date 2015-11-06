#include "View.h"
#include "Geometry.h"
#include <iostream>

using namespace std;

int main(){
    View view;

    view.update_location("Steve", Point(0.0, 0.0));
    view.update_location("Alphonse", Point(4.0, 4.0));
    view.update_location("Roger", Point(50.0, 50.0));
    view.update_location("Dodger", Point(50.0, 50.0));

    view.draw();

    view.set_scale(4.5);

    view.draw();

    view.set_scale(10.0);

    view.draw();

    view.update_remove("Steve");

    view.draw();

    return 0;
}
