/**
 * Slightly modified version of the Arduino Helpers Button Class for
 * tracking the state of a button with debounce logic and timing information.
 * Author: Philip Brown
 * Original Author: Pieter Pas (https://github.com/tttapa)
 * Original Source: https://github.com/tttapa/Arduino-Helpers/blob/master/src/AH/Hardware/Button.hpp
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

const unsigned long DEFAULT_BUTTON_DEBOUNCE_TIME = 25; // milliseconds

// Helper class for a button with debounce logic and state tracking
class Button
{
public:
    Button(uint16_t pin);

    // Initializes the button pin as an input with pullup resistor
    void begin();

    // Sets the inverted flag of the button to invert the button state (active HIGH vs active LOW)
    void setInverted(bool invert = true);

    // The various states of the button
    enum State
    {
        Released = 0b11,  // Button is fully released (high)
        Pressing = 0b10,  // Button is being pushed (high to low transition)
        Pressed = 0b00,   // Button is fully pushed (low)
        Releasing = 0b01, // Button is being released (low to high transition)
    };

    // Update the state of the button with a digital read and debounce logic
    State update();

    // Gets the current debounced state of the button without updating it
    State getState() const;

    // Gets the time in milliseconds since the last button state change
    unsigned long getPrevBounceTime() const;

    // Gets the time in milliseconds between the provided time and the last button state change
    unsigned long getStableTime(unsigned long now) const;

    // Gets the time in milliseconds since the button state has been stable (based on current time)
    unsigned long getStableTime() const;

    // Updates the static debounce time for all Button instances
    static void setDebounceTime(unsigned long debounceTime = DEFAULT_BUTTON_DEBOUNCE_TIME);

    // Gets the current static debounce time for all Button instances
    static unsigned long getDebounceTime();

private:
    uint16_t pin; // Digital pin number associated with the button

    uint8_t debounced; // The 2 bit representation of the debounced button state

    bool bouncing; // Indicates if the button is currently bouncing

    bool prevInput; // The previous raw input read from the button pin

    bool inverted; // Indicates if the button logic is inverted (active high vs active low)

    unsigned long prevBounceTime; // The last time the debounced button state changed

    static unsigned long debounceTime;
};

#endif
