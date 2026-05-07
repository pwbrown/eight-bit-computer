#include "Button.h"

// Initialize the static debounc time variable
unsigned long Button::debounceTime = DEFAULT_BUTTON_DEBOUNCE_TIME;

// Constructor to initialize the button with all default states
Button::Button(uint16_t pin) : pin(pin), debounced(0b11), bouncing(true), prevInput(HIGH), inverted(false), prevBounceTime(0) {}

// Initialize button pin as input with pull up resistor
void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
}

// Sets the inverted flag
void Button::setInverted(bool invert)
{
    inverted = invert;
}

// Reads the state of the button and updates the internal state
Button::State Button::update()
{
    // Read the current button state and the current time
    bool input = digitalRead(pin) ^ inverted;
    unsigned long now = millis();

    // Check if enough time has passed since the last bounce
    if (bouncing)
    {
        bouncing = now - prevBounceTime <= debounceTime;
    }

    // Shift the debounce state one bit to the left to either append the
    // new state if not bouncing, or repeat the old state if bouncing
    bool prevState = debounced & 0b01;
    bool newState = bouncing ? prevState : input;
    debounced = prevState << 1 | newState;

    // Check if input state has changed
    if (input != prevInput)
    {
        bouncing = true;
        prevInput = input;
        prevBounceTime = now;
    }
    return getState();
}

// Gets the current state of the button
Button::State Button::getState() const
{
    return static_cast<State>(debounced);
}

// Gets the previous bounce time
unsigned long Button::getPrevBounceTime() const
{
    return prevBounceTime;
}

// Returns the difference between the last debounce time and a
// reference time representing how long the button has been
// in a stable state
unsigned long Button::getStableTime(unsigned long now) const
{
    return now - prevBounceTime;
}

// Returns the stable time based on the current millis value
unsigned long Button::getStableTime() const
{
    return getStableTime(millis());
}

// Sets the debounce time threshold for all buttons
void Button::setDebounceTime(unsigned long debounceTime)
{
    Button::debounceTime = debounceTime;
}

// Returns the current debounce time setting for all buttons
unsigned long Button::getDebounceTime()
{
    return Button::debounceTime;
}