#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

namespace Controller {
    void setup();  // Setup the entire controller system (input, output, screen, etc.)
    void update(); // Update the controller state (called on every loop iteration)
}

#endif // CONTROLLER_H