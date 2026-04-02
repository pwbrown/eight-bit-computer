#include <Arduino.h>

// Pin definitions for all input and output connections
const int SHIFT_DATA_PIN = 2;  // Entry point to shift register data chain
const int SHIFT_CLOCK_PIN = 3; // Shared clock pin for all shift registers
const int SHIFT_LATCH_PIN = 4; // Shared latch pin for all shift registers
const int DATA_ENABLE_PIN = 5; // Active LOW pin to enable the address and data shift registers
const int CTRL_ENABLE_PIN = 6; // Active LOW pin to enable the control shift register
const int CLOCK_MODE_PIN = 7;  // Attached to the clock mode switch to detect if the clock is in astable or monostable mode
const int RAM_MODE_PIN = 8;    // Attached to the RAM mode switch to detect if the RAM is in run or program mode
const int BUTTON_1_PIN = 9;   // Button 1 for user input
const int BUTTON_2_PIN = 10;  // Button 2 for user input
const int BUTTON_3_PIN = 11;  // Button 3 for user input
const int BUTTON_4_PIN = 12;  // Button 4 for user input

// Control Register Bit Definitions
const byte CONTROL_WORD_ENABLE_BIT         = 1 << 0; // Controls the output enable pins for the computer's control word (EEPROMS)
const byte MEMORY_ADDR_REGISTER_ENABLE_BIT = 1 << 1; // Controls the output enable pin for the computer's memory address register (RAM)
const byte MEMORY_WRITE_BIT                = 1 << 2; // Controls the write enable pin for the computer's RAM
const byte RESET_BUTTON_BIT                = 1 << 3; // Directly controls the reset button for the computer

// OLED Display will use A4 and A5 for I2C communication and will be powered directly by the arduino

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}