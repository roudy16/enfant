#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

class Controller {
public:
    Controller();

    // create View object, run the program by acccepting user commands, then destroy View object
    void run();

};

#endif // CONTROLLER_H
