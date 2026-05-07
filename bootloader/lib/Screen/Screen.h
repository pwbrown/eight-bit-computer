/**
 * The Screen namespace contains functions for interacting with the OLED
 * screen hardware. The bootloader uses a specific layout for the screen consisting of
 * 4 corner buttons labels and up to 2 middle labels. This namespace provides simple
 * helper functions specifically designed to interact with this layout.
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Types.h> // Contains the Corner enum definition

using namespace Types;

namespace Screen
{
    void setup(); // Setup the screen hardware
    void clear(); // Clear the screen contents

    void clearCornerLabel(Corner corner);                   // Clear the label in the specified corner
    void drawCornerLabel(Corner corner, const char *label); // Clear and draw a label in the specified corner

    void clearMiddleLabel(bool top = true);                   // Clear the middle label (top or bottom)
    void drawMiddleLabel(const char *label, bool top = true); // Clear and draw the middle label (top or bottom)
}

#endif // SCREEN_H