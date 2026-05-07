/**
 * The Output namespace contains functions for interacting with all bootloader
 * outputs via the shift register chain, and provides helper functions for setting
 * the values that will be pushed to the shift registers.
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>

namespace Output
{
    void setup();  // Initialize output shift register pins
    void update(); // Update the shift registers with the current data values and control settings

    void setDataOutput(bool enable);    // Enables/disables data shift register output
    void setControlOutput(bool enable); // Enables/disables control shift register output

    void setRamAddress(uint8_t address); // Sets the RAM address to be written to
    void setRamValue(uint8_t value);     // Sets the RAM value to be written

    void setResetButton(bool pressed); // Presses/releases the computer reset button through the control register
    void setRamWrite(bool enabled);    // Enables/disables the RAM write bit through the control register
}

#endif // OUTPUT_H