#include "Input.h"
#include "Settings.h" // Pin definitions

using namespace Input;

// Declare all input buttons and switches
Button topLeftButton = Button(TL_BUTTON_PIN);
Button topRightButton = Button(TR_BUTTON_PIN);
Button bottomLeftButton = Button(BL_BUTTON_PIN);
Button bottomRightButton = Button(BR_BUTTON_PIN);
Button ramModeSwitch = Button(RAM_MODE_PIN);
Button clockModeSwitch = Button(CLOCK_MODE_PIN);

// Setup all input buttons and switches
void Input::setup()
{
    // Initialize all buttons and switches
    topLeftButton.begin();
    topRightButton.begin();
    bottomLeftButton.begin();
    bottomRightButton.begin();
    ramModeSwitch.begin();
    clockModeSwitch.begin();
}

// Update the internal state of all buttons and switches
void Input::update()
{
    topLeftButton.update();
    topRightButton.update();
    bottomLeftButton.update();
    bottomRightButton.update();
    ramModeSwitch.update();
    clockModeSwitch.update();
}

// Returns the button state for a given corner
Button::State Input::getButtonState(Corner corner)
{
    switch (corner)
    {
    case TL:
        return topLeftButton.getState();
    case TR:
        return topRightButton.getState();
    case BL:
        return bottomLeftButton.getState();
    case BR:
        return bottomRightButton.getState();
    default:
        return Button::Released; // Default to released for invalid button IDs
    }
}

// Returns the time in milliseconds that the button has been in a stable state for a given corner
unsigned long Input::getButtonStableTime(Corner corner)
{
    switch (corner)
    {
    case TL:
        return topLeftButton.getStableTime();
    case TR:
        return topRightButton.getStableTime();
    case BL:
        return bottomLeftButton.getStableTime();
    case BR:
        return bottomRightButton.getStableTime();
    default:
        return 0; // Default to 0 for invalid button IDs
    }
}

// Detects if a button is being pressed (transitioning from released to pressed)
bool Input::isPressingButton(Corner corner)
{
    return getButtonState(corner) == Button::Pressing;
}

// Detects if a button is being pressed beyond a certain time threshold
bool Input::isHoldingButton(Corner corner)
{
    return (
        getButtonState(corner) == Button::Pressed &&
        getButtonStableTime(corner) >= BUTTON_HOLD_THRESHOLD_MS);
}

// Detects if a button is being released (transitioning from pressed to released)
bool Input::isReleasingButton(Corner corner)
{
    return getButtonState(corner) == Button::Releasing;
}

// Indicates if the RAM mode switch is in RUN mode (vs. PROGRAM)
bool Input::isRamInRunMode()
{
    return ramModeSwitch.getState() == Button::Pressed;
}

// Indicates if the clock mode switch is in MANUAL mode (vs. AUTOMATIC)
bool Input::isClockInManualMode()
{
    return clockModeSwitch.getState() == Button::Pressed;
}