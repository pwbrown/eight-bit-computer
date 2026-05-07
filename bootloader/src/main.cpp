/**
 * The bootloader utilizes 3 serial in parallel out (SIPO)
 * shift registers in a chain, inputs coming from the 8 bit
 * computer clock and RAM mode switches, and a small OLED display
 * with 4 corner buttons. With those inputs and outputs, the bootloader
 * can detect user actions via the corner buttons to select different
 * predefined programs and write them to the 8 bit computer through
 * the shift register chain. This file is the entrypoint for the bootloader
 * and contains most of the logic while deferring the complexity of I/O operations
 * to classes/namespaces in the libs directory
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#include <Arduino.h>

#include "Settings.h" // Holds behavior settings
#include "Programs.h" // The full list of predefined programs
#include "Input.h"    // For reading button/switch states
#include "Output.h"   // For setting output states/values to control the computer
#include "Screen.h"   // For drawing to the OLED screen
#include "Timer.h"    // For handling timing of button holds and program writing sequence
#include "Types.h"    // For custom types used in the program

using namespace Types;

// Define the possible screen views
enum View
{
  PROGRAM_LIST,   // View for browsing the list of predefined programs
  PROGRAM_EDITOR, // View for editing the variables of a selected program
};

// Variables for controller the current view and related state
View currentView = PROGRAM_LIST;                     // Start in the program list view
bool isDisplayingError = false;                      // Whether we are currently displaying an error message
bool showingBRLabel = true;                          // Indicates if the bottom right label is currently showing
Timer<millis> errorDisplayTimeout = Timer<millis>(); // Timer for how long to display error messages
Timer<millis> adjustmentTimer = Timer<millis>();     // Timer for controlling variable adjustment speed when holding up/down buttons

// Variables for controlling program and variable selection
uint8_t currentProgramIndex = 0;  // Index of the currently selected program
uint8_t currentVariableIndex = 0; // Index of the currently selected variable within the program

// Variables for controlling the program write sequence
bool isWritingProgram = false;              // Whether the bootloader is currently writing a program to the computer
uint8_t currentWriteStep = 0;               // The current step in the write sequence
uint8_t currentWriteAddress = 0;            // The current RAM address being written to during the write sequence
Timer<millis> writeTimer = Timer<millis>(); // Timer used by the write sequence to add non-blocking delays in betweeen steps

/********************** VIEW DRAWING ***************************/

// Draws the currently selected program to the screen
void drawCurrentProgramInListView()
{
  Program currentProgram = PROGRAMS[currentProgramIndex];

  // Draw the program name on the top middle label
  Screen::drawMiddleLabel(currentProgram.getName(), true);

  // Draw the other labels based on whether the program has variables
  bool hasVariables = currentProgram.getVariableCount() > 0;
  if (hasVariables)
  {
    // Make sure the "Edit" label is showing
    if (!showingBRLabel)
    {
      Screen::drawCornerLabel(BR, "Edit");
      showingBRLabel = true;
    }
    // Draw the program variables on the bottom middle label
    char *variablesString = currentProgram.getVariablesString();
    Screen::drawMiddleLabel(variablesString, false);
    free(variablesString);
  }
  else
  {
    // Make sure the "Edit" label is not showing
    if (showingBRLabel)
    {
      Screen::clearCornerLabel(BR);
      showingBRLabel = false;
    }
    // Clear the bottom middle label since there are no variables to show
    Screen::clearMiddleLabel(false);
  }
}

// Draws the currently selected program variable to the bottom middle label on the screen
void drawCurrentProgramVariableInEditorView()
{
  Program currentProgram = PROGRAMS[currentProgramIndex];
  char *variableString = currentProgram.getVariableString(currentVariableIndex);
  if (variableString != nullptr)
  {
    Screen::drawMiddleLabel(variableString, false);
    free(variableString);
  }
  else
  {
    Screen::clearMiddleLabel(false);
  }
}

/************************* VIEW NAVIGATION *********************/

// Navigates to the program list view and updates the screen accordingly
void gotoProgramListView()
{
  currentView = PROGRAM_LIST;
  showingBRLabel = false;
  Screen::drawCornerLabel(TL, "Next");
  Screen::drawCornerLabel(BL, "Previous");
  Screen::drawCornerLabel(TR, "Write");
  Screen::clearCornerLabel(BR);
  drawCurrentProgramInListView();
}

// Navigates to the program editor view and updates the screen accordingly
void gotoProgramEditorView(bool resetVariableIndex = true)
{
  currentView = PROGRAM_EDITOR;
  if (resetVariableIndex)
  {
    currentVariableIndex = 0;
  }
  adjustmentTimer.start(ADJUSTMENT_HOLD_DELAY_MS);
  Screen::drawCornerLabel(TL, "Increase");
  Screen::drawCornerLabel(BL, "Decrease");
  Screen::drawCornerLabel(TR, "Next");
  Screen::drawCornerLabel(BR, "Back");
  drawCurrentProgramVariableInEditorView();
}

/********************** ERROR MESSAGE HANDLING ************************/

// Shows an error message to the middle of the screen
void showErrorMessage(const char *errorMessage)
{
  isDisplayingError = true;
  errorDisplayTimeout.start(ERROR_DISPLAY_DURATION_MS);
  // Reveal the "Accept" label in the top right and hide all other labels
  Screen::drawCornerLabel(TR, "Accept");
  Screen::clearCornerLabel(TL);
  Screen::clearCornerLabel(BL);
  Screen::clearCornerLabel(BR);
  // Show the error message
  Screen::drawMiddleLabel("ERROR", true);
  Screen::drawMiddleLabel(errorMessage, false);
}

// Hides the error message and restores the previous view
void hideErrorMessage()
{
  isDisplayingError = false;
  switch (currentView)
  {
  case PROGRAM_LIST:
    gotoProgramListView();
    break;
  case PROGRAM_EDITOR:
    gotoProgramEditorView(false);
    break;
  }
}

/****************** PROGRAM WRITE SEQUENCE HANDLING *****************/

// Setup and start the program write sequence.
// Also checks the computer state and reports errors if necessary
void startProgramWriteSequence()
{
  // Check the computer clock state
  if (!Input::isClockInManualMode())
  {
    showErrorMessage("Set Clock to MAN");
    return;
  }
  // Check the computer RAM mode
  if (!Input::isRamInRunMode())
  {
    showErrorMessage("Set RAM to RUN");
    return;
  }
  // Reset the sequence variables
  currentWriteStep = 0;
  currentWriteAddress = 0;
  // Reset the output register values
  Output::setRamWrite(false);
  Output::setResetButton(false);
  Output::setRamAddress(0);
  Output::setRamValue(0);
  Output::update();
  // Update the screen to indicate writing state with abort button
  Screen::clearCornerLabel(TL);
  Screen::clearCornerLabel(BL);
  Screen::clearCornerLabel(BR);
  Screen::drawCornerLabel(TR, "Abort");
  Screen::drawMiddleLabel("Writing...", false);
  // Initialize the write timer with a quarter second start delay
  writeTimer.start(250);
  // Start the write sequence
  isWritingProgram = true;
}

// Called whenever the program write sequence stops either naturally
// or forcefully to perform the final cleanup steps
void onProgramWriteStop()
{
  // Reset back to the program list view
  gotoProgramListView();
  // Stop the program write sequence
  isWritingProgram = false;
}

// Manually aborts the write sequence and disconnects the bootloader
// from the computer
void abortProgramWriteSequence()
{
  // Disconnect from the computer
  Output::setDataOutput(false);
  Output::setControlOutput(false);
  onProgramWriteStop();
}

// Checks if the provided step number aligns with the current write sequence
// step, and if so it will auto increment the current write sequence step
bool isCurrentStep(int step)
{
  if (step == currentWriteStep)
  {
    currentWriteStep++;
    return true;
  }
  return false;
}

// Handles the logic for the current write sequence step
void handleNextWriteSequenceStep()
{
  // Ignore this pass if the write timer has not triggered
  if (!writeTimer.check())
  {
    return;
  }

  // Let each step use and increment this step tracker
  int step = 0;

  // Step 1: Enable the control register output to all the computer
  // to disable the computers's control word and RAM address
  // register to prevent bus and input contention
  if (isCurrentStep(step++))
  {
    Output::setControlOutput(true);
    writeTimer.start(10);
    return;
  }

  // Step 2: Enable the data shift register output to let the RAM
  // address and RAM value reach the RAM module for writing.
  if (isCurrentStep(step++))
  {
    Output::setDataOutput(true);
    writeTimer.start(10);
    return;
  }

  // Step 3: Write the program to RAM one address at a time

  // Incrment the current step based on the RAM address and the number of
  // steps per each write cycle (3)
  step += (currentWriteAddress * 3);

  // Only write up to address 15 (16 byte RAM)
  if (currentWriteAddress < 16)
  {
    // Step 3a: Apply the current RAM address and program instruction
    // byte at the current address to the data output register
    if (isCurrentStep(step++))
    {
      byte instructionBinary = PROGRAMS[currentProgramIndex].getInstructionByte(currentWriteAddress);
      Output::setRamAddress(currentWriteAddress);
      Output::setRamValue(instructionBinary);
      Output::update();
      writeTimer.start(10);
      return;
    }

    // Step 3b: Enable the RAM write flag to start writing the value to RAM
    // at the address set in the previous step
    if (isCurrentStep(step++))
    {
      Output::setRamWrite(true);
      Output::update();
      writeTimer.start(10);
      return;
    }

    // Step 3c: Disable the RAM write flag to stop writing to RAM
    if (isCurrentStep(step++))
    {
      Output::setRamWrite(false);
      Output::update();
      currentWriteAddress++; // Increment the current write address
      writeTimer.start(10);
      return;
    }
  }

  // Step 4: Reset the computer

  // Step 4a: Press the reset button
  if (isCurrentStep(step++))
  {
    Output::setResetButton(true);
    Output::update();
    writeTimer.start(5);
    return;
  }

  // Step 4b: Release the reset button
  if (isCurrentStep(step++))
  {
    Output::setResetButton(false);
    Output::update();
    writeTimer.start(5);
    return;
  }

  // Step 5: Disable the data shift register output
  if (isCurrentStep(step++))
  {
    Output::setDataOutput(false);
    writeTimer.start(10);
    return;
  }

  // Step 6: Disable the contorl shift register output and stop writing
  if (isCurrentStep(step++))
  {
    Output::setControlOutput(false);
    onProgramWriteStop();
    return;
  }
}

/************************* VIEW HANDLING ************************/

// Handle the logic for the program list view on each loop iteration
void handleProgramListView()
{
  // "Next" button
  if (Input::isPressingButton(TL))
  {
    currentProgramIndex = (currentProgramIndex + 1) % NUM_PROGRAMS; // Increment program index and wrap around using modulo
    drawCurrentProgramInListView();
  }
  // "Previous" button
  else if (Input::isPressingButton(BL))
  {
    currentProgramIndex = (currentProgramIndex - 1 + NUM_PROGRAMS) % NUM_PROGRAMS; // Decrement program index and wrap around using modulo
    drawCurrentProgramInListView();
  }
  // "Write" button
  else if (Input::isPressingButton(TR))
  {
    startProgramWriteSequence();
  }
  // "Edit" button (only showing if the current program has variables)
  else if (Input::isPressingButton(BR))
  {
    if (PROGRAMS[currentProgramIndex].getVariableCount() > 0)
    {
      gotoProgramEditorView();
    }
  }
}

// Handle the logic for the program editor view on each loop iteration
void handleProgramEditorView()
{
  // Handle "Increase" and "Decrease" Buttons
  bool isIncreasing = Input::isPressingButton(TL) || Input::isHoldingButton(TL);
  if (isIncreasing || Input::isPressingButton(BL) || Input::isHoldingButton(BL))
  {
    bool isHolding = Input::isHoldingButton(TL) || Input::isHoldingButton(BL);
    // Only adjust if we are pressing a button (not holding) or adjustment timer has triggered
    if (!isHolding || adjustmentTimer.check())
    {
      // Calculate min and max allowed values based on instruction type
      int16_t minValue = -128;
      int16_t maxValue = 255;
      if (PROGRAMS[currentProgramIndex].isVariableHalfByte(currentVariableIndex))
      {
        minValue = 0;
        maxValue = 15;
      }
      // Calculate new variable value and wrap if necessary based on min and max
      int16_t newValue = PROGRAMS[currentProgramIndex].getVariableValue(currentVariableIndex) + (isIncreasing ? 1 : -1);
      if (newValue > maxValue)
      {
        newValue = minValue;
      }
      else if (newValue < minValue)
      {
        newValue = maxValue;
      }
      // Set the new value and draw it on the screen
      PROGRAMS[currentProgramIndex].setVariableValue(currentVariableIndex, newValue);
      drawCurrentProgramVariableInEditorView();
    }
  }
  // Handle "Next" button
  else if (Input::isPressingButton(TR))
  {
    // Increment and wrap variable index and then draw it on the screen
    currentVariableIndex = (currentVariableIndex + 1) % PROGRAMS[currentProgramIndex].getVariableCount();
    drawCurrentProgramVariableInEditorView();
  }
  // Handle "Back" button
  else if (Input::isPressingButton(BR))
  {
    gotoProgramListView();
  }
}

/************************** ARDUINO ENTRYPOINT *************************/

/** Setup function (runs once at startup) */
void setup()
{
  // Initialize serial communication for debugging (baud rate must match the platformio.ini file)
  Serial.begin(57600);

  // Setup all modules
  Output::setup();
  Input::setup();
  Screen::setup();

  // Goto the initial view
  gotoProgramListView();
}

/** Main loop function (runs repeatedly after setup) */
void loop()
{
  // Update all the input states
  Input::update();

  // An active error message takes precedence over all other interactions
  if (isDisplayingError)
  {
    // Check if the "Accept" button is pressed or the error has timed out
    if (Input::isPressingButton(TR) || errorDisplayTimeout.check())
    {
      hideErrorMessage();
    }
    return;
  }

  // Handle the logic for an active program write sequence
  if (isWritingProgram)
  {
    // Handle the "Abort" button for manually stopping the write sequence
    if (Input::isPressingButton(TR))
    {
      abortProgramWriteSequence();
    }
    else
    {
      handleNextWriteSequenceStep();
    }
    return;
  }

  // Route to the current view's logic handler
  switch (currentView)
  {
  case PROGRAM_LIST:
    handleProgramListView();
    break;
  case PROGRAM_EDITOR:
    handleProgramEditorView();
    break;
  }
}