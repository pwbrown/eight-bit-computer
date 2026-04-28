#include "Input.h"
#include "Settings.h" // Pin definitions

// Setup all input button and switches
void Input::setup() {
    // Setup all buttons
    topLeftButton = Button(TL_BUTTON_PIN);
    topRightButton = Button(TR_BUTTON_PIN);
    bottomLeftButton = Button(BL_BUTTON_PIN);
    bottomRightButton = Button(BR_BUTTON_PIN);
    ramModeSwitch = Button(RAM_MODE_PIN);
    clockModeSwitch = Button(CLOCK_MODE_PIN);

    // Initialize all buttons and switches
    topLeftButton.begin();
    topRightButton.begin();
    bottomLeftButton.begin();
    bottomRightButton.begin();
    ramModeSwitch.begin();
    clockModeSwitch.begin();
}

// Update the internal state of all buttons and switches
void Input::update() {
    topLeftButton.update();
    topRightButton.update();
    bottomLeftButton.update();
    bottomRightButton.update();
    ramModeSwitch.update();
    clockModeSwitch.update();
}

// Detects if the top button button is being pressed (transitioning from released to pressed)
bool Input::isPressingTopLeftButton() {
    return topLeftButton.getState() == Button::Pressing;
}

// Detects if the top right button is being pressed (transitioning from released to pressed)
bool Input::isPressingTopRightButton() {
    return topRightButton.getState() == Button::Pressing;
}

// Detects if the bottom left button is being pressed (transitioning from released to pressed)
bool Input::isPressingBottomLeftButton() {
    return bottomLeftButton.getState() == Button::Pressing;
}

// Detects if the bottom right button is being pressed (transitioning from released to pressed)
bool Input::isPressingBottomRightButton() {
    return bottomRightButton.getState() == Button::Pressing;
}

// Detects if the RAM mode switch is in run mode
bool Input::isRamInRunMode() {
    return ramModeSwitch.getState() == Button::Pressed;
}

// Detects if the clock mode switch is in manual mode
bool Input::isClockInManualMode() {
    return clockModeSwitch.getState() == Button::Pressed;
}