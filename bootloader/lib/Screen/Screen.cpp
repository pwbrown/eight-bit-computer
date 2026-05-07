#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>

#include "Screen.h"
#include "Settings.h" // Defines screen settings

// Declare display object for the OLED screen
SSD1306AsciiAvrI2c display;

// Track lengths of labels for each corner to help with cleanup
uint8_t tlCurrentLength = 0;
uint8_t trCurrentLength = 0;
uint8_t blCurrentLength = 0;
uint8_t brCurrentLength = 0;

/******************* UTILITIES ********************/

// Get the length of the current label in a corner
uint8_t getCurrentLabelLength(Corner corner)
{
    switch (corner)
    {
    case TL:
        return tlCurrentLength;
    case TR:
        return trCurrentLength;
    case BL:
        return blCurrentLength;
    case BR:
        return brCurrentLength;
    }
}

// Set the length of the current label in a corner
void setCurrentLabelLength(Corner corner, uint8_t length)
{
    switch (corner)
    {
    case TL:
        tlCurrentLength = length;
        break;
    case TR:
        trCurrentLength = length;
        break;
    case BL:
        blCurrentLength = length;
        break;
    case BR:
        brCurrentLength = length;
        break;
    }
}

// Setup the screen hardware
void Screen::setup()
{
    display.begin(&Adafruit128x64, SCREEN_ADDRESS);
    display.setFont(Adafruit5x7);
    display.set1X();
    clear();
}

// Clear the screen contents
void Screen::clear()
{
    display.clear();
}

// Clears the label in a specific corner and resets the corner label length
void Screen::clearCornerLabel(Corner corner)
{
    // Check the current label length.
    int labelLength = getCurrentLabelLength(corner);
    if (labelLength == 0)
    {
        return;
    }

    // Determine the column (in pixels) and row (in character rows) based on the corner
    int col = 0;
    int row = 0;
    if (corner == BL || corner == BR)
    {
        row = display.displayRows() - 1; // Align text to the bottom edge
    }
    if (corner == TR || corner == BR)
    {
        col = display.displayWidth() - display.fieldWidth(getCurrentLabelLength(corner)) - 1; // Align text to the right edge
    }

    // Clear the text from the screen and reset the corner label length to 0 (no text)
    display.clearField(col, row, getCurrentLabelLength(corner));
    setCurrentLabelLength(corner, 0);
}

// Draws a label in a specific corner. Clears the corner first if there is an existing label.
void Screen::drawCornerLabel(Corner corner, const char *label)
{
    // Clear any existing text
    clearCornerLabel(corner);

    // Get the label length in characters and pixels
    uint8_t labelLengthInChars = strlen(label);
    uint8_t labelLengthInPixels = display.fieldWidth(labelLengthInChars);

    // Calculate the column (in pixels) and row (in character rows) based on the corner
    uint8_t col = 0; // Column in pixels
    uint8_t row = 0; // Row based on character height (not pixels)
    if (corner == BL || corner == BR)
    {
        row = display.displayRows() - 1; // Align text to the bottom edge
    }
    if (corner == TR || corner == BR)
    {
        col = display.displayWidth() - labelLengthInPixels - 1; // Align text to the right edge
    }

    // Set the cursor position and print the label to the screen
    display.setCursor(col, row);
    display.print(label);

    // Update the current label length for the corner
    setCurrentLabelLength(corner, labelLengthInChars);
}

// Clears the text in the middle of the screen (either top or bottom)
void Screen::clearMiddleLabel(bool top)
{
    // Calculate top middle row
    int row = display.displayRows() / 2 - 1;
    if (!top)
    {
        row += 1; // Move down a row for the bottom middle label
    }

    // Clear the entire row
    display.clearField(0, row, display.displayWidth() / display.fieldWidth(1));
}

// Draws the label in the middle of the screen (either top or bottom). Clears the middle label first
void Screen::drawMiddleLabel(const char *label, bool top)
{
    // Clear the middle label first
    clearMiddleLabel(top);

    // Get the label length in characters and pixels
    uint8_t labelLengthInChars = strlen(label);
    uint8_t labelLengthInPixels = display.fieldWidth(labelLengthInChars);

    // Calculate the column (in pixels) and row (in character rows)
    uint8_t row = display.displayRows() / 2 - 1;                            // Centered vertically
    uint8_t col = (display.displayWidth() / 2) - (labelLengthInPixels / 2); // Centered horizontally
    if (!top)
    {
        row += 1; // Move down a row for the bottom middle label
    }

    // Set the cursor position and print the label to the screen
    display.setCursor(col, row);
    display.print(label);
}