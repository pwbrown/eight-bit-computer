#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "Button.h"

class Input {
    public:
        // Setup all input pins and initial states
        static void setup();

        // Update the state of all inputs
        static void update();

        // Helpers for detecting if buttons presses are being initiated
        static bool isPressingTopLeftButton();
        static bool isPressingTopRightButton();
        static bool isPressingBottomLeftButton();
        static bool isPressingBottomRightButton();

        // Helpers for detecting the current state of input switches
        static bool isRamInRunMode();
        static bool isClockInManualMode();

    private:
        // Buttons for user inputs
        static Button topLeftButton;
        static Button topRightButton;
        static Button bottomLeftButton;
        static Button bottomRightButton;
        
        // Switches from the computer for mode detection
        static Button ramModeSwitch;
        static Button clockModeSwitch;
};


#endif // INPUT_H