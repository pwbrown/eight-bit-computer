#include <Arduino.h>
#include "Programs.h" // All programs that can be loaded into RAM
#include "Output.h"   // Controls all boot loader outputs (data and control shift registers)
#include "Input.h"    // Controls all boot loader inputs (buttons and computer switches)
#include "Timer.h"    // Non-blocking timer implementation for managing timing

// Timers
Timer<millis> writeTimer; // Program write timer to manage timing of the write program sequence steps

/** Function declarations */
void startProgramWriteSequence();
void abortProgramWriteSequence();
void onProgramWriteStop();
void runNextWriteStep();
bool isCurStep(int step);

/** Programming state variables */
uint8_t curProgramIndex = 1;    // The index of the current program to write from the PROGRAMS array in Programs.h
bool isWritingProgram = false;  // Whether we are currently in the process of writing a program to RAM
int curWriteStep = 0;           // The current step in the write program sequence
int curWriteAddr = 0;           // The current RAM address being written to in the write program sequence

/** Setup function (runs once at startup) */
void setup() {
  // Initialize serial communication for debugging (baud rate must match the platformio.ini file)
  Serial.begin(57600);

  // Initialize outputs first
  Output::setup();

  // Initialize inputs
  Input::setup();
}

/** Main loop function (runs repeatedly after setup) */
void loop() {
  /** Update all of the input states at the beginning of each loop iteration */
  Input::update();

  if (Input::isPressingTopLeftButton()) {
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
  /** Reset output register values */
  Output::setRamWrite(false);
  Output::setResetButton(false);
  Output::setRamAddress(0);
  Output::setRamValue(0);
  Output::update();
  /** Initialize write timer with a 500ms start delay */
  writeTimer.start(500);
  /** Start the program write sequence */
  isWritingProgram = true;
}

/** Aborts the current program write sequence */
void abortProgramWriteSequence() {
  Serial.println("-- Aborting program write sequence");
  isWritingProgram = false;
  /** Quickly detach the bootloader from the computer */
  Output::disableDataOutput();
  Output::disableControlOutput();
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
    Output::enableControlOutput();
    writeTimer.start(10);
    return;
  }

  // Step 2: Enable the output for the data shift registers now that the bus
  // and address register are disabled.
  if (isCurStep(step++)) {
    Output::enableDataOutput();
    writeTimer.start(10);
    return;
  }

  step += (curWriteAddr * 3); // Increment by the RAM address value multiplied by the write cycle steps
  
  // Step 3: Write the program to RAM one address at a time
  if (curWriteAddr < 16) {
    // Step 3a: Handle selecting the current RAM address and RAM value
    if (isCurStep(step++)) {
      uint8_t instructionBin = 0; // getProgramInstructionBinary(curProgramIndex, curWriteAddr);
      Output::setRamAddress(curWriteAddr);
      Output::setRamValue(instructionBin);
      Output::update();
      writeTimer.start(10);
      return;
    }
  
    // Step 3b: Set the ram write flag to true to start writing the value to RAM
    if (isCurStep(step++)) {
      Output::setRamWrite(true);
      Output::update();
      writeTimer.start(10);
      return;
    }
  
    // Step 3c: Set the ram write flag to false to finish writing the value to RAM
    if (isCurStep(step++)) {
      Output::setRamWrite(false);
      Output::update();
      curWriteAddr++; // Increment the current RAM address being written to for the next write cycle
      writeTimer.start(50);
      return;
    }
  }
  
  // Step 4: Press the computer reset button
  if (isCurStep(step++)) {
    Output::setResetButton(true);
    Output::update();
    writeTimer.start(10);
    return;
  }
  
  // Step 5: Release the computer reset button
  if (isCurStep(step++)) {
    Output::setResetButton(false);
    Output::update();
    writeTimer.start(50);
    return;
  }

  // Step 6: Disable the data shift register outputs now that we are done writing the program
  if (isCurStep(step++)) {
    Output::disableDataOutput();
    writeTimer.start(10);
    return;
  }

  // Step 7: Disable the control shift register output to restore control to the computer
  if (isCurStep(step++)) {
    Output::disableControlOutput();
    writeTimer.start(10);
    return;
  }

  // Step 8: Stop writing the program
  if (isCurStep(step++)) {
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