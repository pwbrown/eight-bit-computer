#include "Controller.h"
#include "Programs.h"
#include "Screen.h"
#include "Output.h"
#include "Input.h"
#include "Timer.h"
#include "SharedTypes.h"

using namespace Controller;
using namespace SharedTypes;

// Function declarations for internal use
void handleNextWriteStep();
void startProgramWriteSequence();
void abortProgramWriteSequence();

enum View
{
  PROGRAM_LIST,
  PROGRAM_EDITOR,
};

const int ADJUSTMENT_HOLD_DELAY_MS = 50;

// Initialize static variables
uint8_t currentProgramIndex = 0;
uint8_t currentVariableIndex = 0;
uint8_t currentWriteStep = 0;
uint8_t currentWriteAddress = 0;
bool isWritingProgram = false;
bool isDisplayingError = false;
View currentView = PROGRAM_LIST;
Timer<millis> writeTimer = Timer<millis>();
Timer<millis> adjustmentTimer = Timer<millis>();
Timer<millis> errorDisplayTimeout = Timer<millis>();

// Draws the currently select program index name and variables to the middle label
void drawSelectedProgramInListView()
{
  Screen::drawMiddleLabel(PROGRAMS[currentProgramIndex].name);
  char *variablesString = PROGRAMS[currentProgramIndex].printVariables();
  Screen::drawMiddleLabel(variablesString, false);
  free(variablesString);
}

// Switches to the program list view and updates the screen accordingly
void switchToProgramListView()
{
  currentView = PROGRAM_LIST;
  Screen::drawCornerLabel(TL, "Previous");
  Screen::drawCornerLabel(BL, "Next");
  Screen::drawCornerLabel(TR, "Write");
  Screen::drawCornerLabel(BR, "Edit");
  drawSelectedProgramInListView();
}

// Draws the currently selected program variable name and value to the middle bottom label
void drawSelectedProgramVariableInEditorView()
{
  char *variableString = PROGRAMS[currentProgramIndex].variables[currentVariableIndex].print();
  Screen::drawMiddleLabel(variableString, false);
  free(variableString);
}

// Handles switching to and drawing the program editor view
void switchToProgramEditorView()
{
  currentView = PROGRAM_EDITOR;
  currentVariableIndex = 0;
  Screen::drawCornerLabel(TL, "Up");
  Screen::drawCornerLabel(BL, "Down");
  Screen::drawCornerLabel(TR, "Next");
  Screen::drawCornerLabel(BR, "Back");
  drawSelectedProgramVariableInEditorView();
}

// Handles the actions of the program list view
void handleProgramListView()
{
  // Handle the error display
  if (isDisplayingError) {
    if (Input::isPressingButton(TR) || errorDisplayTimeout) {
      isDisplayingError = false;
      // Restore labels
      Screen::drawCornerLabel(TR, "Write");
      drawSelectedProgramInListView();
    }
    return;
  }
  // Limited control access while writing the program
  if (isWritingProgram)
  {
    // Allow the write sequence to be aborted
    if (Input::isPressingButton(TR))
    {
      abortProgramWriteSequence();
    }
    handleNextWriteStep();
    return;
  }
  // Goto the next program
  if (Input::isPressingButton(BL))
  {
    currentProgramIndex = (currentProgramIndex + 1) % NUM_PROGRAMS; // Increment program index and wrap around using modulo
    drawSelectedProgramInListView();
  }
  // Goto the previous program
  else if (Input::isPressingButton(TL))
  {
    currentProgramIndex = (currentProgramIndex - 1 + NUM_PROGRAMS) % NUM_PROGRAMS; // Decrement program index and wrap around using modulo
    drawSelectedProgramInListView();
  }
  // Start writing the program to RAM
  else if (Input::isPressingButton(TR))
  {
    startProgramWriteSequence();
  }
  // Switch to the program editor view
  else if (Input::isPressingButton(BR) && PROGRAMS[currentProgramIndex].variables[0].name != nullptr)
  {
    switchToProgramEditorView();
  }
}

void handleProgramEditorView()
{
  // Adjust current variable value and display it to the screen
  if (
    Input::isPressingButton(TL) ||
    Input::isPressingButton(BL) ||
    Input::isHoldingButton(TL) ||
    Input::isHoldingButton(BL)
  ) {
    bool isIncrementing = Input::isPressingButton(TL) || Input::isHoldingButton(TL);
    bool isHolding = Input::isHoldingButton(TL) || Input::isHoldingButton(BL);
    if (!isHolding || adjustmentTimer) {
      int16_t newValue = PROGRAMS[currentProgramIndex].variables[currentVariableIndex].value + (isIncrementing ? 1 : -1);
      if (newValue > 255) {
        newValue = -128; // Wrap around to minimum value
      } else if (newValue < -128) {
        newValue = 255; // Wrap around to maximum value
      }
      setProgramVariable(currentProgramIndex, currentVariableIndex, newValue);
      drawSelectedProgramVariableInEditorView();
    }

  }
  // Switch to the next variable
  else if (Input::isPressingButton(TR)) {
    uint8_t nextVariableIndex = (currentVariableIndex + 1) % MAX_PROGRAM_VARIABLES; // Increment variable index and wrap around using modulo
    // Wrap back to first variable if we hit an unused variable
    if (PROGRAMS[currentProgramIndex].variables[nextVariableIndex].name == nullptr) {
      nextVariableIndex = 0;
    }
    currentVariableIndex = nextVariableIndex;
    drawSelectedProgramVariableInEditorView();
  }
  // Go back to the program list view
  else if (Input::isPressingButton(BR)) {
    switchToProgramListView();
  }
}

// Display an error message in the middle of the screen for a few seconds
void displayError(const char* errorMessage) {
  isDisplayingError = true;
  errorDisplayTimeout.start(3000); // Display the error message for 3 seconds
  Screen::drawCornerLabel(TR, "Accept");
  Screen::drawMiddleLabel("ERROR", true);
  Screen::drawMiddleLabel(errorMessage, false);
}

// Resets and starts the program write sequence
void startProgramWriteSequence()
{
  // Check computer state first
  if (!Input::isClockInManualMode()) {
    displayError("Set CLOCK to MAN");
    return;
  } else if (!Input::isRamInRunMode()) {
    displayError("Set RAM to RUN");
    return;
  }
  // Reset sequence variables
  currentWriteStep = 0;
  currentWriteAddress = 0;
  // Reset output register values
  Output::setRamWrite(false);
  Output::setResetButton(false);
  Output::setRamAddress(0);
  Output::setRamValue(0);
  Output::update();
  // Initialize the write timer with a 500ms start delay
  writeTimer.start(500);
  // Start the program write sequence
  isWritingProgram = true;
  // Update screen to indicate writing state
  Screen::drawCornerLabel(TR, "Abort");
  Screen::drawMiddleLabel("Writing...", false);
}

// Called after the program write sequence stops or the sequence is manually aborted
void onProgramWriteStop()
{
  // Redraw the currently selected program and upper right label
  Screen::drawCornerLabel(TR, "Write");
  drawSelectedProgramInListView();
}

// Handles aborting the write sequence and quickly detaches from the computer
void abortProgramWriteSequence()
{
  isWritingProgram = false;
  Output::setDataOutput(false);
  Output::setControlOutput(false);
  onProgramWriteStop();
}

// Helper to check if the indicated step number is the current step in the program write sequence
bool isCurrentStep(int step)
{
  if (step == currentWriteStep)
  {
    currentWriteStep++;
    return true;
  }
  return false;
}

// Handles the logic for the step in the program write sequence
void handleNextWriteStep()
{
  // Check the current write timer to see if we can process to the next step in the write sequence.
  if (!writeTimer)
  {
    return;
  }

  // Track the sequence step and let each step increment it
  int step = 0;

  // Step 1: Enable control output.
  // This takes control of the computer and disables the control word
  // and address register to prevent bus and input contention while we
  // apply the new RAM address and RAM values during writing.
  if (isCurrentStep(step++))
  {
    Output::setControlOutput(true);
    writeTimer.start(10);
    return;
  }

  // Step 2: Enable the output for the data shift registers now that the bus
  // and address register are disabled.
  if (isCurrentStep(step++))
  {
    Output::setDataOutput(true);
    writeTimer.start(10);
    return;
  }

  step += (currentWriteAddress * 3); // Increment by the RAM address value multiplied by the write cycle steps

  // Step 3: Write the program to RAM one address at a time
  if (currentWriteAddress < 16)
  {
    // Step 3a: Handle selecting the current RAM address and RAM value
    if (isCurrentStep(step++))
    {
      byte instructionBin = getProgramInstructionByte(currentProgramIndex, currentWriteAddress);
      Output::setRamAddress(currentWriteAddress);
      Output::setRamValue(instructionBin);
      Output::update();
      writeTimer.start(10);
      return;
    }

    // Step 3b: Set the ram write flag to true to start writing the value to RAM
    if (isCurrentStep(step++))
    {
      Output::setRamWrite(true);
      Output::update();
      writeTimer.start(10);
      return;
    }

    // Step 3c: Set the ram write flag to false to finish writing the value to RAM
    if (isCurrentStep(step++))
    {
      Output::setRamWrite(false);
      Output::update();
      currentWriteAddress++; // Increment the current RAM address being written to for the next write cycle
      writeTimer.start(50);
      return;
    }
  }

  // Step 4: Press the computer reset button
  if (isCurrentStep(step++))
  {
    Output::setResetButton(true);
    Output::update();
    writeTimer.start(10);
    return;
  }

  // Step 5: Release the computer reset button
  if (isCurrentStep(step++))
  {
    Output::setResetButton(false);
    Output::update();
    writeTimer.start(50);
    return;
  }

  // Step 6: Disable the data shift register outputs now that we are done writing the program
  if (isCurrentStep(step++))
  {
    Output::setDataOutput(false);
    writeTimer.start(10);
    return;
  }

  // Step 7: Disable the control shift register output to restore control to the computer
  if (isCurrentStep(step++))
  {
    Output::setControlOutput(false);
    writeTimer.start(10);
    return;
  }

  // Step 8: Stop writing the program
  if (isCurrentStep(step++))
  {
    isWritingProgram = false;
    onProgramWriteStop(); // Report program write sequence stop
    return;
  }
}

// Initialize everything
void Controller::setup()
{
  // Initialize the outputs, inputs, and screen
  Output::setup();
  Input::setup();
  Screen::setup();

  // Begin the adjustment timer
  adjustmentTimer.start(ADJUSTMENT_HOLD_DELAY_MS);

  switchToProgramListView(); // Start in the program list view
}

// Updates the controller and should be called on every iteration of the main loop
void Controller::update()
{
  // Update all input states
  Input::update();

  // Handle update logic based on the current view
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