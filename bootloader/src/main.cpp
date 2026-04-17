#include <Arduino.h>
#include "Button.h" // Button class in the "lib" folder. Modified from the Arduino Helpers library
#include "Timer.h"  // Timer class in the "lib" folder. Modified from the Arduino Helpers library

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

// Timers
Timer<millis> writeTimer; // Program write timer to manage timing of the write program sequence steps

/** Function declarations */
void startProgramWriteSequence();
void abortProgramWriteSequence();
void onProgramWriteStop();
void runNextWriteStep();
bool isCurStep(int step);
void updateShiftRegisters();
void enableDataOutput(bool enable = true);
void enableControlOutput(bool enable = true);

/** Internal state variables */
uint8_t ramAddress = 0;         // The currently selected RAM address (0-15)]
uint8_t ramValue = 0;           // The current value to be written to RAM at the selected address (0-255)
bool pressResetButton = false;  // Whether the reset button should be pressed
bool enableRamWrite = false;    // Whether the RAM should write the ramValue to the ramAddress

/** Programming state variables */
uint8_t programToWrite[16] = {  // The current program to write to RAM
  0b11101101,
  0b11101111,
  0b10010110,
  0b00001101,
  0b00111110,
  0b00011101,
  0b00001100,
  0b01100001,
  0b00011100,
  0b10110000,
  0b11001101,
  0b11110000,
  0b00001000,
  0b00000000,
  0b00010000,
  0b00000111,
};
bool isWritingProgram = false;  // Whether we are currently in the process of writing a program to RAM
int curWriteStep = 0;           // The current step in the write program sequence
int curWriteAddr = 0;           // The current RAM address being written to in the write program sequence

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
}

/** Main loop function (runs repeatedly after setup) */
void loop() {
  /** Get top left button state */
  Button::State topLeftState = topLeftButton.update();
  bool isPressingTopLeftButton = (topLeftState == Button::Pressing);

  if (isPressingTopLeftButton) {
    if (!isWritingProgram) {
      Serial.println("Detected top left button press, starting program write sequence");
      startProgramWriteSequence();
    } else {
      Serial.println("Detected top left button press. Aborting current program write sequence.");
      abortProgramWriteSequence();
    }
  }

  /** Check if we are currently writing a program to RAM */
  if (isWritingProgram) {
    runNextWriteStep();
  }
}

/** Sets up all variables for the program write sequence */
void startProgramWriteSequence() {
  /** Reset sequence variables */
  curWriteStep = 0;
  curWriteAddr = 0;
  /** Reset shift register values */
  enableRamWrite = false;
  pressResetButton = false;
  ramAddress = 0;
  ramValue = 0;
  updateShiftRegisters();
  /** Reset write timer to an interval of 0: Executes immediately */
  writeTimer.start(0);
  /** Start the program write sequence */
  isWritingProgram = true;
}

/** Aborts the current program write sequence */
void abortProgramWriteSequence() {
  Serial.println("-- Aborting program write sequence");
  isWritingProgram = false;
  /** Quickly detach the bootloader from the computer */
  enableDataOutput(false);
  enableControlOutput(false);
  /** Report program write sequence stop */
  onProgramWriteStop();
}

/**
 * Called when the program write sequence stops either normally or due to an abort.
 */
void onProgramWriteStop() {
  /** Not implemented yet */
}

/**
 * Runs the next step in the program write sequence as long as the write timer has triggered.
 * Each step in the sequence can set a non-blocking time delay before the next step.
 */
void runNextWriteStep() {
  // Check the current write timer to see if we can process to the next step in the write sequence.
  if (!writeTimer) {
    return;
  }

  // Track the sequence step and let each step increment it
  int step = 0;

  // Step 1: Enable control output.
  // This takes control of the computer and disables the control word
  // and address register to prevent bus and input contention while we
  // apply the new RAM address and RAM values during writing.
  if (isCurStep(step++)) {
    Serial.println("-- Enabling Control Register Output");
    enableControlOutput(true);
    writeTimer.start(500);
    return;
  }

  // Step 2: Enable the output for the data shift registers now that the bus
  // and address register are disabled.
  if (isCurStep(step++)) {
    Serial.println("-- Enabling Data Register Output");
    enableDataOutput(true);
    writeTimer.start(500);
    return;
  }

  step += (curWriteAddr * 3); // Increment by the RAM address value multiplied by the write cycle steps
  
  // Step 3: Write the program to RAM one address at a time
  if (curWriteAddr < 16) {
    // Step 3a: Handle selecting the current RAM address and RAM value
    if (isCurStep(step++)) {
      Serial.print("-- Writing value for RAM address ");
      Serial.println(curWriteAddr, BIN);
      Serial.print("   -- Setting RAM address to ");
      Serial.print(curWriteAddr, BIN);
      Serial.print(" and RAM value to ");
      Serial.println(programToWrite[curWriteAddr], BIN);
      ramAddress = curWriteAddr;
      ramValue = programToWrite[curWriteAddr];
      updateShiftRegisters();
      writeTimer.start(100);
      return;
    }
  
    // Step 3b: Set the ram write flag to true to start writing the value to RAM
    if (isCurStep(step++)) {
      Serial.println("   -- Enabling RAM write");
      enableRamWrite = true;
      updateShiftRegisters();
      writeTimer.start(100);
      return;
    }
  
    // Step 3c: Set the ram write flag to false to finish writing the value to RAM
    if (isCurStep(step++)) {
      Serial.println("   -- Disabling RAM write");
      enableRamWrite = false;
      updateShiftRegisters();
      curWriteAddr++; // Increment the current RAM address being written to for the next write cycle
      writeTimer.start(100);
      return;
    }
  }
  
  // Step 4: Press the computer reset button
  if (isCurStep(step++)) {
    Serial.println("-- Pressing computer reset button");
    pressResetButton = true;
    updateShiftRegisters();
    writeTimer.start(500);
    return;
  }
  
  // Step 5: Release the computer reset button
  if (isCurStep(step++)) {
    Serial.println("-- Releasing computer reset button");
    pressResetButton = false;
    updateShiftRegisters();
    writeTimer.start(500);
    return;
  }

  // Step 6: Disable the data shift register outputs now that we are done writing the program
  if (isCurStep(step++)) {
    Serial.println("-- Disabling data register output");
    enableDataOutput(false);
    writeTimer.start(500);
    return;
  }

  // Step 7: Disable the control shift register output to restore control to the computer
  if (isCurStep(step++)) {
    Serial.println("-- Disabling control register output");
    enableControlOutput(false);
    writeTimer.start(500);
    return;
  }

  // Step 8: Stop writing the program
  if (isCurStep(step++)) {
    Serial.println("-- Finished writing program to RAM");
    isWritingProgram = false;
    onProgramWriteStop(); // Report program write sequence stop
    return;
  }
}

/**
 * Checks if the step provided matches the current step in the write program sequence.
 * If it does, it advances the current step and returns true. Otherwise, it returns false
 * and does not advance the current step.
 */
bool isCurStep(int step) {
  if (step == curWriteStep) {
    curWriteStep++;
    return true;
  }
  return false;
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