#include <Arduino.h>
#include "Button.h"

#define TL_BUTTON_PIN 11  // Top Left Button
#define BL_BUTTON_PIN 12  // Bottom Left Button
#define TR_BUTTON_PIN 8   // Top Right Button
#define BR_BUTTON_PIN 9   // Bottom Right Button
#define CLOCK_MODE_PIN 10 // Clock Mode Switch
#define RAM_MODE_PIN 7    // RAM Mode Switch

// Create button instances for all buttons and switches
Button topLeftButton(TL_BUTTON_PIN);
Button bottomLeftButton(BL_BUTTON_PIN);
Button topRightButton(TR_BUTTON_PIN);
Button bottomRightButton(BR_BUTTON_PIN);
Button clockModeSwitch(CLOCK_MODE_PIN);
Button ramModeSwitch(RAM_MODE_PIN);

// Function declarations
bool isClockModeManual();
bool isRamModeRun();

// Previous states
bool prevClockManual = false;
bool prevRamRun = false;

void setup() {
  Serial.begin(57600);

  // Initialize all buttons and switches
  topLeftButton.begin();
  bottomLeftButton.begin();
  topRightButton.begin();
  bottomRightButton.begin();
  clockModeSwitch.begin();
  ramModeSwitch.begin();

  // Initialize previous states
  prevClockManual = isClockModeManual();
  prevRamRun = isRamModeRun();
}

void loop() {
  // Update all button states
  Button::State tlButtonState = topLeftButton.update();
  Button::State blButtonState = bottomLeftButton.update();
  Button::State trButtonState = topRightButton.update();
  Button::State brButtonState = bottomRightButton.update();

  // Update and get resolved states for input switches
  bool isClockManual = isClockModeManual();
  bool isRamRun = isRamModeRun();

  // Report switch state changes
  if (isClockManual != prevClockManual) {
    Serial.print("Clock Mode: ");
    Serial.println(isClockManual ? "Manual" : "Automatic");
    prevClockManual = isClockManual;
  }
  if (isRamRun != prevRamRun) {
    Serial.print("RAM Mode: ");
    Serial.println(isRamRun ? "Run" : "Program");
    prevRamRun = isRamRun;
  }

  // Top Left Button
  if (tlButtonState == Button::State::Pressing) {
    Serial.println("Top Left Button Was Pressed");
  }
  if (tlButtonState == Button::State::Releasing) {
    Serial.println("Top Left Button Was Released");
  }
  // Bottom Left Button
  if (blButtonState == Button::State::Pressing) {
    Serial.println("Bottom Left Button Was Pressed");
  }
  if (blButtonState == Button::State::Releasing) {
    Serial.println("Bottom Left Button Was Released");
  }
  // Top Right Button
  if (trButtonState == Button::State::Pressing) {
    Serial.println("Top Right Button Was Pressed");
  }
  if (trButtonState == Button::State::Releasing) {
    Serial.println("Top Right Button Was Released");
  }
  // Bottom Right Button
  if (brButtonState == Button::State::Pressing) {
    Serial.println("Bottom Right Button Was Pressed");
  }
  if (brButtonState == Button::State::Releasing) {
    Serial.println("Bottom Right Button Was Released");
  }
}

// Checks if the clock is in "manual" mode vs "automatic" mode
bool isClockModeManual() {
  Button::State clockState = clockModeSwitch.update();
  return clockState == Button::Pressed || clockState == Button::Pressing;
}

// Checks if the ram is in "run" mode vs "program" mode
bool isRamModeRun() {
  Button::State ramState = ramModeSwitch.update();
  return ramState == Button::Pressed || ramState == Button::Pressing;
}