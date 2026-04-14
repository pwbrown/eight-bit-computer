// Slightly modified version of the Arduino Helpers Button Class
// Author: Pieter Pas (https://github.com/tttapa)
// Original source: https://github.com/tttapa/Arduino-Helpers/blob/master/src/AH/Hardware/Button.cpp

#include "Button.h"

Button::Button(uint16_t pin) : pin(pin) {}

// Initialize button pin as input with pull up resistor
void Button::begin() {
    pinMode(pin, INPUT_PULLUP);
}

// Set the invert flag of the button to invert the button state (pressed is high vs low)
void Button::invert() {
    state.invert = !state.invert;
}

// Reads the state of the button and updates the internal state
Button::State Button::update() {
    // Read the current button state and the current time
    bool input = digitalRead(pin) ^ state.invert;
    unsigned long now = millis();

    // Check if enough time has passed since the last bounce
    if (state.bouncing) {
        state.bouncing = now - state.prevBounceTime <= debounceTime;
    }
    
    // Shift the debounce state one bit to the left to either append the
    // new state if not bouncing, or repeat the old state if bouncing
    bool prevState = state.debounced & 0b01;
    bool newState = state.bouncing ? prevState : input;
    state.debounced = prevState << 1 | newState;

    // Check if input state has changed
    if (input != state.prevInput) {
        state.bouncing = true;
        state.prevInput = input;
        state.prevBounceTime = now;
    }
    return getState();
}

// Gets the current state of the button
Button::State Button::getState() const {
    return static_cast<State>(state.debounced);
}

// Gets the previous bounce time
unsigned long Button::prevBounceTime() const {
    return state.prevBounceTime;
}

// Returns the difference between the last debounce time and a
// reference time representing how long the button has been
// in a stable state
unsigned long Button::stableTime(unsigned long now) const {
    return now - prevBounceTime();
}

// Returns the stable time based on the current millis value
unsigned long Button::stableTime() const {
    return stableTime(millis());
}

// Sets the debounce time threshold for all buttons
void Button::setDebounceTime(unsigned long debounceTime) {
    Button::debounceTime = debounceTime;
}

// Returns the current debounce time setting for all buttons
unsigned long Button::getDebounceTime() {
    return Button::debounceTime;
}

unsigned long Button::debounceTime = BUTTON_DEBOUNCE_TIME;