#include "Arduino.h"

// Pin definitions
const int SHIFT_DATA_PIN = 2;  // 16-bit shift register data pin
const int SHIFT_CLOCK_PIN = 3; // 16-bit shift register clock pin
const int SHIFT_LATCH_PIN = 4; // 16-bit shift register latch pin
const int EEPROM_IO_PINS[] = {12, 11, 10, 9, 8, 7, 6, 5}; // EEPROM I/O pins (D0-D7)
const int EEPROM_WRITE_PIN = 13; // EEPROM Write Enable pin

// EEPROM write shortcuts
const int ENABLE_WRITE = LOW;  // Active LOW for write enable
const int DISABLE_WRITE = HIGH; // Active LOW for write enable

// Initial setup for all EEPROM pins
void setupEEPROMPins() {
    // Set shift register pins and initial states
    digitalWrite(SHIFT_DATA_PIN, LOW);
    digitalWrite(SHIFT_CLOCK_PIN, LOW);
    digitalWrite(SHIFT_LATCH_PIN, LOW);
    pinMode(SHIFT_DATA_PIN, OUTPUT);
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_LATCH_PIN, OUTPUT);

    // Set EEPROM Write Enable pin
    digitalWrite(EEPROM_WRITE_PIN, DISABLE_WRITE); // Disable writing by default
    pinMode(EEPROM_WRITE_PIN, OUTPUT);

    // No need to set pin mode for IO pins here since the read and write operations will handle that
}

// Sets the pin mode for all IO pins of the EEPROM
void setModeForEEPROMIOPins(int mode) {
    for (int i = 0; i < 8; i += 1) {
        pinMode(EEPROM_IO_PINS[i], mode);
    }
}

// Sets the address for the EEPROM and the output enable pin using the shift registers
void setEEPROMAddressAndOutput(int address, bool enableOutput) {
    // Shift out the most significant address bits and the output enable pin (active low)
    int addressMsbAndOutput = (address >> 8) | (enableOutput ? 0x00 : 0x80);
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, addressMsbAndOutput);
    
    // Shift out the least significant address bits next
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, address);

    // Toggle the shift latch to expose the data in the shift register
    digitalWrite(SHIFT_LATCH_PIN, HIGH);
    digitalWrite(SHIFT_LATCH_PIN, LOW);
}

// Reads a single byte from an address on the EEPROM
byte readEEPROMByte(int address) {
    // Set the direction for all IO pins
    setModeForEEPROMIOPins(INPUT);

    // Set the address and enable the output (from EEPROM)
    setEEPROMAddressAndOutput(address, true);

    // Read the data backwards into a byte starting with the Most significant bit
    byte data = 0;
    for (int i = 7; i >= 0; i -= 1) {
        data = (data << 1) + digitalRead(EEPROM_IO_PINS[i]);
    }

    return data;
}

// Write a single byte to an address on the EEPROM
void writeEEPROMByte(int address, byte data) {
    // Set the address first and disable the output (from EEPROM)
    setEEPROMAddressAndOutput(address, false);

    // Set the direction for all IO pins
    setModeForEEPROMIOPins(OUTPUT);

    // Set the data on the IO pins one bit at a time
    for (int i = 0; i < 8; i += 1) {
        digitalWrite(EEPROM_IO_PINS[i], data & 1);
        data = data >> 1;
    }

    // Toggle the write enable pin to write to the EEPROM
    digitalWrite(EEPROM_WRITE_PIN, ENABLE_WRITE);
    delayMicroseconds(1);
    digitalWrite(EEPROM_WRITE_PIN, DISABLE_WRITE);

    // Additional delay to allow EEPROM to settle
    delay(10);
}

// Prints the contents of the first n bytes of the EEPROM to the Serial output
void printEEPROMContentsToSerial(int numBytes = 256) {
    Serial.println("EEPROM Contents:");
    for (int base = 0; base < numBytes; base += 16) {
        // Read 16 bytes worth of data into an array
        byte data[16];
        for (int offset = 0; offset < 16; offset += 1) {
            data[offset] = readEEPROMByte(base + offset);
        }

        // Construct data string
        char buf[60];
        sprintf(
            buf,
            // Format: "base_address: byte_1 byte_2 ... byte_n"
            "%03x:   %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            // Arguments
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]
        );

        // Print string to serial
        Serial.println(buf);
    }
}