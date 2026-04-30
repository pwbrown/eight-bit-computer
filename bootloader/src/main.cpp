#include <Arduino.h>
#include "Controller.h"

/** Setup function (runs once at startup) */
void setup() {
  // Initialize serial communication for debugging (baud rate must match the platformio.ini file)
  Serial.begin(57600);

  Controller::setup();
}

/** Main loop function (runs repeatedly after setup) */
void loop() {
  Controller::update();
}