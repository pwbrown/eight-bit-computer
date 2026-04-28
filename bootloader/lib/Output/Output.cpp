#include "Output.h"
#include "Settings.h" // Pin definitions and control bit values

// Setup all output controller pins and initial states
void Output::setup() {
    // Setup shift register enable pins and set them to disabled by default
    disableDataOutput();
    disableControlOutput();
    pinMode(DATA_ENABLE_PIN, OUTPUT);
    pinMode(CTRL_ENABLE_PIN, OUTPUT);

    // Setup shift register pins and initial states
    digitalWrite(SHIFT_DATA_PIN, LOW);
    digitalWrite(SHIFT_LATCH_PIN, LOW);
    digitalWrite(SHIFT_CLOCK_PIN, LOW);
    pinMode(SHIFT_DATA_PIN, OUTPUT);
    pinMode(SHIFT_LATCH_PIN, OUTPUT);
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);

    // Set initial values for the shift registers to ensure known state on startup
    update();
}

// Updates the values of the shift registers based on the current static variable values
void Output::update() {
    // Push the RAM address first since it's the furthest down the shift register chain
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, ramAddress);
    
    // Push the RAM value next
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, ramValue);
    
    // Construct the control byte
    uint8_t controlByte = 0;
    // Control word address (A10) bit (always set to 1)
    controlByte |= (1 << CTRL__CTRL_WORD_ADDR_BIT);
    // Computer reset button (active high)
    controlByte |= ((pressingResetButton ? 1 : 0) << CTRL__RESET_BUTTON_BIT);
    // RAM write multiplexer strobe bit (active high)
    controlByte |= ((enablingRamWrite ? 1 : 0) << CTRL__RAM_WRITE_MUX_STROBE_BIT);
    // RAM address register output enable bit (active low, but always explicitly disabled)
    controlByte |= (1 << CTRL__RAM_ADDR_REG_ENABLE_BIT);

    // Push the control byte last
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, controlByte);

    // Toggle the shift latch to apply all new values simultaneously
    digitalWrite(SHIFT_LATCH_PIN, HIGH);
    digitalWrite(SHIFT_LATCH_PIN, LOW);
}

// Enable data output
void Output::enableDataOutput() {
    // Active LOW enable pin
    digitalWrite(DATA_ENABLE_PIN, LOW);
}

// Disable data output
void Output::disableDataOutput() {
    // Active LOW enable pin
    digitalWrite(DATA_ENABLE_PIN, HIGH);
}

// Enable control output
void Output::enableControlOutput() {
    // Active LOW enable pin
    digitalWrite(CTRL_ENABLE_PIN, LOW);
}

// Disable control output
void Output::disableControlOutput() {
    // Active LOW enable pin
    digitalWrite(CTRL_ENABLE_PIN, HIGH);
}

// Set the static ram address
void Output::setRamAddress(uint8_t address) {
    ramAddress = address;
}

// Set the static ram value
void Output::setRamValue(uint8_t value) {
    ramValue = value;
}

// Set the reset button state
void Output::setResetButton(bool pressed) {
    pressingResetButton = pressed;
}

// Set the RAM write enable state
void Output::setRamWrite(bool enabled) {
    enablingRamWrite = enabled;
}