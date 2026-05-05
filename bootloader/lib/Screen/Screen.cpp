#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>

#include "Screen.h"
#include "Settings.h" // Defines screen settings

// Function declarations
uint8_t getCurrentLabelLength(Corner corner);
void setCurrentLabelLength(Corner corner, uint8_t length);

// Declare display object for the OLED screen
SSD1306AsciiAvrI2c display;

// Track lengths of labels for each corner to help with cleanup
uint8_t tlCurrentLength = 0;
uint8_t trCurrentLength = 0;
uint8_t blCurrentLength = 0;
uint8_t brCurrentLength = 0;

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
    uint8_t labelWidthPx = display.fieldWidth(labelLength);

    uint8_t col = 0;   // Column in pixels
    uint8_t row = 0;   // Row based on character height (not pixels)
    if (corner == BL || corner == BR) {
        row = display.displayRows() - 1; // Align text to the bottom edge
    }

    // Clear any existing text
    int currentLabelLength = getCurrentLabelLength(corner);
    if (currentLabelLength > 0) {
        if (corner == TR || corner == BR) {
            col = display.displayWidth() - display.fieldWidth(currentLabelLength) - 1;
        }
        display.clearField(col, row, currentLabelLength);
    }

    // Draw the new text and update the current label length
    if (corner == TR || corner == BR) {
        col = display.displayWidth() - labelWidthPx - 1; // Align text to the right edge
    }

    display.setCursor(col, row);

    // Draw the label starting at the row and column determined by the corner
    display.print(label);

    // Update the current label length for the corner
    setCurrentLabelLength(corner, labelLength);
}

// Draws the label in the middle of the screen
void Screen::drawMiddleLabel(const char* label, bool top) {
    // Set text size and calculate text bounds based on the text size
    display.set1X();
    uint8_t labelLength = strlen(label);
    uint8_t labelWidthPx = display.fieldWidth(labelLength);
    uint8_t row = display.displayRows() / 2 - 1; // Centered vertically
    uint8_t col = (display.displayWidth() / 2) - (labelWidthPx / 2); // Centered horizontally

    // Move down a row if we are not on the top label
    if (!top) {
        row += 1;
    }

    // Clear the whole line first
    display.clearField(0, row, display.displayWidth() / display.fieldWidth(1));

    display.setCursor(col, row);

    display.print(label);
}

// Get the length of the current label in a corner
uint8_t getCurrentLabelLength(Corner corner) {
    return corner == TL ? tlCurrentLength :
           corner == TR ? trCurrentLength :
           corner == BL ? blCurrentLength : brCurrentLength;
}

// Set the length of the current label in a corner
void setCurrentLabelLength(Corner corner, uint8_t length) {
    if (corner == TL) {
        tlCurrentLength = length;
    } else if (corner == TR) {
        trCurrentLength = length;
    } else if (corner == BL) {
        blCurrentLength = length;
    } else if (corner == BR) {
        brCurrentLength = length;
    }
}