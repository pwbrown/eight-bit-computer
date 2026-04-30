#ifndef INPUT_H
#define INPUT_H

#include "Button.h"
#include "SharedTypes.h"  // Contains the Corner enum definition

using namespace SharedTypes;

namespace Input {
    void setup();  // Setup all input buttons and switches
    void update(); // Update the internal state of all buttons and switches

    // Check button state
    Button::State getButtonState(Corner corner);      // Gets the current button state
    unsigned long getButtonStableTime(Corner corner); // Gets the time in milliseconds that the button has been in a stable state
    bool isPressingButton(Corner corner);  // Detects if the button is transitioning from released to pressed
    bool isHoldingButton(Corner corner);   // Detects if the button is currently being considered "held"
    bool isReleasingButton(Corner corner); // Detects if the button is transitioning from pressed to released

    // Computer switch states
    bool isRamInRunMode();      // Indicates if the RAM mode switch is in RUN mode (vs. PROGRAM)
    bool isClockInManualMode(); // Indicates if the clock mode switch is in MANUAL mode (vs. AUTOMATIC)
}

#endif // INPUT_H