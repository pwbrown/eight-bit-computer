#include <Arduino.h>
#include "Button.h" // Button class in the "lib" folder. Modified from the Arduino Helpers library

// All pin definitions for the project
#define SHIFT_DATA_PIN 2  // Data pin for the shift register chain
#define SHIFT_LATCH_PIN 3 // Latch pin for all shift registers
#define SHIFT_CLOCK_PIN 4 // Clock pin for all shift registers
#define DATA_ENABLE_PIN 5 // Connected to both data shift registers to enable/disable data output
#define CTRL_ENABLE_PIN 6 // Connected to the control shift register to enable/disable control signal output
#define RAM_MODE_PIN 7    // Connected to the RAM mode switch on the computer to detect current mode ("Run" vs "Program")
#define TR_BUTTON_PIN 8   // Connected to the top right button next to the screen
#define BR_BUTTON_PIN 9   // Connected to the bottom right button next to the screen
#define CLOCK_MODE_PIN 10 // Connected to the clock mode switch on the computer to detect current clock mode ("Auto" vs "Manual")
#define TL_BUTTON_PIN 11  // Connected to the top left button next to the screen
#define BL_BUTTON_PIN 12  // Connected to the bottom left button next to the screen

// Control shift register bit definitions
#define CTRL__CTRL_WORD_ADDR_BIT 0       // Connected to the control word EEPROM(s) last address bit (A10)
#define CTRL__RESET_BUTTON_BIT 1         // Connected to the computer's reset button
#define CTRL__RAM_WRITE_MUX_STROBE_BIT 2 // Connected to the RAM write multiplexer's strobe signal pin
#define CTRL__RAM_ADDR_REG_ENABLE_BIT 3  // Connected to the RAM address register's output enable pin

// Button/Switch definitions and setup function
Button topLeftButton(TL_BUTTON_PIN);
Button topRightButton(TR_BUTTON_PIN);
Button bottomLeftButton(BL_BUTTON_PIN);
Button bottomRightButton(BR_BUTTON_PIN);
Button ramModeSwitch(RAM_MODE_PIN);
Button clockModeSwitch(CLOCK_MODE_PIN);

/** Function declarations */
void updateShiftRegisters();
void enableDataOutput(bool enable = true);
void enableControlOutput(bool enable = true);

void waitForButtonPress(Button& button);

/** Internal state variables */
uint8_t ramAddress = 0;         // The currently selected RAM address (0-15)]
uint8_t ramValue = 0;           // The current value to be written to RAM at the selected address (0-255)
bool pressResetButton = false;  // Whether the reset button should be pressed
bool enableRamWrite = false;    // Whether the RAM should write the ramValue to the ramAddress

/** Setup function (runs once at startup) */
void setup() {
  // Initialize serial communication for debugging (baud rate must match the platformio.ini file)
  Serial.begin(57600);

  // Setup shift register enable pins and set them to disabled by default
  enableDataOutput(false);
  enableControlOutput(false);
  pinMode(DATA_ENABLE_PIN, OUTPUT);
  pinMode(CTRL_ENABLE_PIN, OUTPUT);

  // Setup shift register pins and initial states
  digitalWrite(SHIFT_DATA_PIN, LOW);
  digitalWrite(SHIFT_LATCH_PIN, LOW);
  digitalWrite(SHIFT_CLOCK_PIN, LOW);
  pinMode(SHIFT_DATA_PIN, OUTPUT);
  pinMode(SHIFT_LATCH_PIN, OUTPUT);
  pinMode(SHIFT_CLOCK_PIN, OUTPUT);

  // Initialize all buttons and switches
  topLeftButton.begin();
  topRightButton.begin();
  bottomLeftButton.begin();
  bottomRightButton.begin();
  ramModeSwitch.begin();
  clockModeSwitch.begin();

  // Set initial values for the shift registers to ensure known state on startup
  updateShiftRegisters();


  // Testing
  Serial.println("Starting test suite...");

  // Test 1: Enable the control output and check default state
  Serial.println("Press button to enable control output...");
  waitForButtonPress(topLeftButton);
  enableControlOutput(true);
  Serial.println("Control output enabled.");

  // Test 2: Toggle the reset button bit and check output
  Serial.println("Press button to toggle reset button bit...");
  waitForButtonPress(topLeftButton);
  pressResetButton = !pressResetButton;
  updateShiftRegisters();
  pressResetButton = !pressResetButton; // Reset back to default state for next tests
  updateShiftRegisters();
  Serial.println("Reset button bit toggled.");

  // Test 3: Toggle the RAM write strobe bit and check output
  Serial.println("Press button to toggle RAM write strobe bit...");
  waitForButtonPress(topLeftButton);
  enableRamWrite = !enableRamWrite;
  updateShiftRegisters();
  enableRamWrite = !enableRamWrite; // Reset back to default state for next tests
  updateShiftRegisters();
  Serial.println("RAM write strobe bit toggled.");

  // Test 4: Disable control output to restore control back to computer
  Serial.println("Press button to disable control output...");
  waitForButtonPress(topLeftButton);
  enableControlOutput(false);
  Serial.println("Control output disabled. Test suite complete.");
}

/** Main loop function (runs repeatedly after setup) */
void loop() {
  // Not using yet
}

/**
 * Updates the shift register chain (control bits, RAM address, and RAM value) by
 * shifting out the appropriate bits starting with the furthest down the chain.
 * Finishes by toggling the shared latch pin to simultaneously apply all new values.
 * This function does not update the output enable pins, so the value can be applied
 * in the background even if the shift register outputs are disabled.
 */
void updateShiftRegisters() {
  // Push the RAM address first since it's the furthest down the shift register chain
  shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, ramAddress);
  
  // Push the RAM value next
  shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, ramValue);
  
  // Construct the control byte
  uint8_t controlByte = 0;
  // Control word address (A10) bit (always set to 1)
  controlByte |= (1 << CTRL__CTRL_WORD_ADDR_BIT);
  // Computer reset button (active high)
  controlByte |= ((pressResetButton ? 1 : 0) << CTRL__RESET_BUTTON_BIT);
  // RAM write multiplexer strobe bit (active high)
  controlByte |= ((enableRamWrite ? 1 : 0) << CTRL__RAM_WRITE_MUX_STROBE_BIT);
  // RAM address register output enable bit (active low, but always explicitly disabled)
  controlByte |= (1 << CTRL__RAM_ADDR_REG_ENABLE_BIT);

  // Push the control byte last
  shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, controlByte);

  // Toggle the shift latch to apply all new values simultaneously
  digitalWrite(SHIFT_LATCH_PIN, HIGH);
  digitalWrite(SHIFT_LATCH_PIN, LOW);
}

/**
 * Enables or disables the output of the data shift registers to the computer based on the input parameter.
 * @param enable If true, enables data output; if false, disables data output.
 */
void enableDataOutput(bool enable = true) {
  digitalWrite(DATA_ENABLE_PIN, enable ? LOW : HIGH); // Active LOW enable pin
}

/**
 * Enables or disables the output of the control shift register to the computer based on the input parameter.
 * @param enable If true, enables control signal output; if false, disables control signal output.
 */
void enableControlOutput(bool enable = true) {
  digitalWrite(CTRL_ENABLE_PIN, enable ? LOW : HIGH); // Active LOW enable pin
}

void waitForButtonPress(Button& button) {
  while (button.update() != Button::Pressing) {}
}