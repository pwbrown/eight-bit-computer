// Slightly modified version of the Arduino Helpers Button Class
// Author: Pieter Pas (https://github.com/tttapa)
// Original source: https://github.com/tttapa/Arduino-Helpers/blob/master/src/AH/Hardware/Button.hpp

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

const unsigned long BUTTON_DEBOUNCE_TIME = 25; // milliseconds

class Button {
    public:
        Button(uint16_t pin);

        void begin();

        void invert();

        enum State {
            Released = 0b11,  // Button is fully released (high)
            Pressing = 0b10,  // Button is being pushed (high to low transition)
            Pressed = 0b00,   // Button is fully pushed (low)
            Releasing = 0b01, // Button is being released (low to high transition)
        };

        State update();

        State getState() const;
        
        unsigned long prevBounceTime() const;

        unsigned long stableTime(unsigned long now) const;

        unsigned long stableTime() const;

        static void setDebounceTime(unsigned long debounceTime = BUTTON_DEBOUNCE_TIME);

        static unsigned long getDebounceTime();

    private:
        uint16_t pin;

        struct InternalState {
            InternalState()
                : debounced(0b11),
                  bouncing(true),
                  prevInput(HIGH),
                  invert(false),
                  prevBounceTime(0) {}
            uint8_t debounced : 2;
            bool bouncing : 1;
            bool prevInput : 1;
            bool invert : 1;
            unsigned long prevBounceTime;
        } state;

        static unsigned long debounceTime;
};

#endif
