/**
 * The EEPROM programmer is mostly a recreation of the original Ben Eater
 * logic used for writing to an EEPROM using the breadboard programmer. The
 * only major difference is using an array to store the wiring setup for the
 * EEPROM I/O pins to make the code more dynamic for different wiring setups
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#ifndef EEPROM_PROGRAMMER_H
#define EEPROM_PROGRAMMER_H

#include <Arduino.h>

namespace EEPROMProgrammer
{
    // Initial setup for all EEPROM pins
    void setupPins();

    // Sets the pin mode for all IO pins of the EEPROM
    void setModeForIOPins(int mode);

    // Sets the address for the EEPROM and the output enable pin using the shift registers
    void setAddressAndOutput(int address, bool enableOutput);

    // Reads a single byte from an address on the EEPROM
    byte readByte(int address);

    // Write a single byte to an address on the EEPROM
    void writeByte(int address, byte data);

    // Prints the contents of the first n bytes of the EEPROM to the Serial output
    void printContentsToSerial(int numBytes = 256);
}

#endif // EEPROM_PROGRAMMER_H