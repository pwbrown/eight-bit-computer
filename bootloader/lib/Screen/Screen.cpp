#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>

#include "Screen.h"
#include "Settings.h" // Defines screen settings

#define SCREEN_WIDTH_PX 128
#define SCREEN_HEIGHT_PX 64
#define CHAR_BOX_WIDTH_PX 6 // Width of character and spacing
#define CHAR_BOX_HEIGHT_PX 8 // Height of character and spacing

// Calculate my own row and column max counts
const uint8_t SCREEN_ROWS = SCREEN_HEIGHT_PX / CHAR_BOX_HEIGHT_PX;

// Declare display object for the OLED screen
SSD1306AsciiAvrI2c display;

// Setup the screen hardware
void Screen::setup() {
    display.begin(&Adafruit128x64, SCREEN_ADDRESS);
    display.setFont(Adafruit5x7);
    clear();
}

// Clear the screen contents
void Screen::clear() {
    display.clear();
}

// Draws the label in a specific corner of the screen
void Screen::drawCornerLabel(Corner corner, const char* label) {
    // Set text size and calculate text bounds based on the text size
    display.set1X();

    uint8_t labelLength = strlen(label);
    uint8_t colPx = 0; // Column in pixels
    uint8_t row = 0;   // Row based on character height (not pixels)
    
    if (corner == TR || corner == BR) {
        colPx = SCREEN_WIDTH_PX - (labelLength * CHAR_BOX_WIDTH_PX) - 1; // Align text to the right edge
    }
    if (corner == BL || corner == BR) {
        row = SCREEN_ROWS - 1; // Align text to the bottom edge
    }
    display.setCursor(colPx, row);

    // Draw the label starting at the row and column determined by the corner
    display.print(label);
}

// Draws the label in the middle of the screen
void Screen::drawMiddleLabel(const char* label) {
    // Set text size and calculate text bounds based on the text size
    display.set1X();
    uint8_t labelLength = strlen(label);
    uint8_t colPx = (SCREEN_WIDTH_PX / 2) - (labelLength * CHAR_BOX_WIDTH_PX / 2); // Centered horizontally
    uint8_t row = (SCREEN_ROWS / 2) - 1; // Centered vertically
    display.setCursor(colPx, row);

    display.print(label);
}