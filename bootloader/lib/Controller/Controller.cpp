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

enum View {
    PROGRAM_LIST,
    PROGRAM_EDITOR,
};

struct ViewLabels {
    const char* topLeft;
    const char* topRight;
    const char* bottomLeft;
    const char* bottomRight;
};

// Labels for the program list view
const ViewLabels PROGRAM_LIST_LABELS = {
    topLeft: "Previous", // Goto previous program in the list
    topRight: "Program", // Start writing the currently selected program to RAM
    bottomLeft: "Next",  // Goto next program in the list
    bottomRight: "Edit", // Edit the currently selected program
};

// Labels for the program editor view
const ViewLabels PROGRAM_EDITOR_LABELS = {
    topLeft: "Inc",       // Increase the value of the selected program variable
    topRight: "Next",     // Move to the next variable of the selected program
    bottomLeft: "Dec",    // Decrease the value of the selected program variable
    bottomRight: "Back",  // Return back to the program list view
};

// Initialize static variables
uint8_t currentProgramIndex = 0;
uint8_t currentWriteStep = 0;
uint8_t currentWriteAddress = 0;
bool isWritingProgram = false;
View currentView = PROGRAM_LIST;
Timer<millis> writeTimer = Timer<millis>();

// Draw the labels of a view to the screen
void drawViewLabels(const ViewLabels& labels) {
    Screen::drawCornerLabel(TL, labels.topLeft);
    Screen::drawCornerLabel(TR, labels.topRight);
    Screen::drawCornerLabel(BL, labels.bottomLeft);
    Screen::drawCornerLabel(BR, labels.bottomRight);
}

// Redraws the program list view
void drawProgramListView() {
  Screen::clear();
  drawViewLabels(PROGRAM_LIST_LABELS);
  Screen::drawMiddleLabel(PROGRAMS[currentProgramIndex].name);
}

// Switches to the program list view and updates the screen accordingly
void switchToProgramListView() {
    currentView = PROGRAM_LIST;
    drawProgramListView();
}  

void switchToProgramEditorView() {
    currentView = PROGRAM_EDITOR;
    Screen::clear();
    drawViewLabels(PROGRAM_EDITOR_LABELS);
    Screen::drawMiddleLabel("TODO");
}

void handleProgramListView() {
    // Goto the next program
    if (Input::isPressingButton(BL)) {
        currentProgramIndex = (currentProgramIndex + 1) % NUM_PROGRAMS; // Increment program index and wrap around using modulo
        drawProgramListView();
    }
    // Goto the previous program
    else if (Input::isPressingButton(TL)) {
        currentProgramIndex = (currentProgramIndex - 1 + NUM_PROGRAMS) % NUM_PROGRAMS; // Decrement program index and wrap around using modulo
        drawProgramListView();
    }
    // Start writing the program to RAM
    else if (Input::isPressingButton(TR)) {
        startProgramWriteSequence();
    }

    if (isWritingProgram) {
        handleNextWriteStep();
    }
}

void handleProgramEditorView() {
    // Todo
}

// Resets and starts the program write sequence
void startProgramWriteSequence() {
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
}

// Called after the program write sequence stops or the sequence is manually aborted
void onProgramWriteStop() {
    // Todo
}

// Handles aborting the write sequence and quickly detaches from the computer
void abortProgramWriteSequence() {
    isWritingProgram = false;
    Output::setDataOutput(false);
    Output::setControlOutput(false);
    onProgramWriteStop();
}

// Helper to check if the indicated step number is the current step in the program write sequence
bool isCurrentStep(int step) {
  if (step == currentWriteStep) {
    currentWriteStep++;
    return true;
  }
  return false;
}

// Handles the logic for the step in the program write sequence
void handleNextWriteStep() {
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
  if (isCurrentStep(step++)) {
    Output::setControlOutput(true);
    writeTimer.start(10);
    return;
  }

  // Step 2: Enable the output for the data shift registers now that the bus
  // and address register are disabled.
  if (isCurrentStep(step++)) {
    Output::setDataOutput(true);
    writeTimer.start(10);
    return;
  }

  step += (currentWriteAddress * 3); // Increment by the RAM address value multiplied by the write cycle steps
  
  // Step 3: Write the program to RAM one address at a time
  if (currentWriteAddress < 16) {
    // Step 3a: Handle selecting the current RAM address and RAM value
    if (isCurrentStep(step++)) {
      byte instructionBin = getProgramInstructionByte(currentProgramIndex, currentWriteAddress);
      Output::setRamAddress(currentWriteAddress);
      Output::setRamValue(instructionBin);
      Output::update();
      writeTimer.start(10);
      return;
    }
  
    // Step 3b: Set the ram write flag to true to start writing the value to RAM
    if (isCurrentStep(step++)) {
      Output::setRamWrite(true);
      Output::update();
      writeTimer.start(10);
      return;
    }
  
    // Step 3c: Set the ram write flag to false to finish writing the value to RAM
    if (isCurrentStep(step++)) {
      Output::setRamWrite(false);
      Output::update();
      currentWriteAddress++; // Increment the current RAM address being written to for the next write cycle
      writeTimer.start(50);
      return;
    }
  }
  
  // Step 4: Press the computer reset button
  if (isCurrentStep(step++)) {
    Output::setResetButton(true);
    Output::update();
    writeTimer.start(10);
    return;
  }
  
  // Step 5: Release the computer reset button
  if (isCurrentStep(step++)) {
    Output::setResetButton(false);
    Output::update();
    writeTimer.start(50);
    return;
  }

  // Step 6: Disable the data shift register outputs now that we are done writing the program
  if (isCurrentStep(step++)) {
    Output::setDataOutput(false);
    writeTimer.start(10);
    return;
  }

  // Step 7: Disable the control shift register output to restore control to the computer
  if (isCurrentStep(step++)) {
    Output::setControlOutput(false);
    writeTimer.start(10);
    return;
  }

  // Step 8: Stop writing the program
  if (isCurrentStep(step++)) {
    isWritingProgram = false;
    onProgramWriteStop(); // Report program write sequence stop
    return;
  }
}

// Initialize everything
void Controller::setup() {
    // Initialize the outputs, inputs, and screen
    Output::setup();
    Input::setup();
    Screen::setup();

    switchToProgramListView(); // Start in the program list view
}

// Updates the controller and should be called on every iteration of the main loop
void Controller::update() {
  // Update all input states
  Input::update();
  
  // Handle update logic based on the current view
  switch(currentView) {
    case PROGRAM_LIST:
    handleProgramListView();
    break;
    case PROGRAM_EDITOR:
    handleProgramEditorView();
    break;
  }
}