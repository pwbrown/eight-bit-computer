#include <Arduino.h>

/*************************** CONTROL SIGNAL BIT VALUES **************************/

// Output Module (Numeric Display)
const uint16_t OI =  1 << 0;  // Output In

// B Register
const uint16_t BI =  1 << 1;  // B Register In

// Arithmetic Logic Unit (ALU)
const uint16_t FI =  1 << 2;  // Flags Register In
const uint16_t SU =  1 << 3;  // Subtract Enable
const uint16_t EO =  1 << 4;  // Sum Out (ALU Out)

// A Register
const uint16_t AO =  1 << 5;  // A Register Out
const uint16_t AI =  1 << 6;  // A Register In

// Program Counter
const uint16_t J  =  1 << 7;  // Jump (Counter In)
const uint16_t CO =  1 << 8;  // Counter Out 
const uint16_t CE =  1 << 9;  // Counter Enable (Increment)

// Instruction Register
const uint16_t IO =  1 << 10; // Instruction Register Out
const uint16_t II =  1 << 11; // Instruction Register In

// RAM (Memory)
const uint16_t RO =  1 << 12; // RAM Out
const uint16_t RI =  1 << 13; // RAM In
const uint16_t MI =  1 << 14; // Memory Address Register In

// MISC
const uint16_t HLT = 1 << 15; // Halt Enable (stop the clock)


/************************** Instructions *******************************/

// Definition of a single instruction
struct Instruction {
    char* id;          // Instruction Identifier (ex. ADD)
    bool hasArg;       // Indicates if the instruction has an argument
    uint16_t logic[3]; // Logic values representing T2-T4 of the microcode sequence
};

// List of the first 2 microcode operations for the fetch and decode stages, which are the same for all instructions
const uint16_t FETCH_DECODE[2] = {
    // T0 - Fetch
    CO|MI,
    // T1 - Decode instruction and increment program counter
    RO|II|CE,
};

// The full list of ordered instructions with their control logic (template)
const Instruction INSTRUCTIONS[16] = {
    // Load A: Loads the A Register with a value from RAM at the given address
    {"LDA", true, {IO|MI, RO|AI, 0}},
    // Store A: Stores the value in the A Register into RAM at the given address
    {"STA", true, {IO|MI, AO|RI, 0}},
    // Load Immediate: Loads the given value directly into the A Register
    {"LDI", true, {IO|AI, 0, 0}},
    // Add: Adds the value at the given memory address to the A Register and stores the result in the A Register
    {"ADD", true, {IO|MI, RO|BI, EO|AI|FI}},
    // Add Immediate: Adds the given value to the A Register and stores the result in the A Register
    {"ADI", true, {IO|BI, EO|AI|FI, 0}},
    // Subtract: Subtracts the value at the given memory address from the A Register and stores the result in the A Register
    {"SUB", true, {IO|MI, RO|BI, EO|SU|AI|FI}},
    // Subtract Immediate: Subtracts the given value from the A Register and stores the result in the A Register
    {"SUI", true, {IO|BI, EO|SU|AI|FI, 0}},
    // Jump: Performs an unconditional jump to the given instruction address
    {"JMP", true, {IO|J, 0, 0}},
    // Jump on Carry: Performs a jump if the carry flag is enabled
    {"JOC", true, {0, 0, 0}},
    // Jump not Carry: Performs a jump if the carry flag is not enabled
    {"JNC", true, {0, 0, 0}},
    // Jump on Zero: Performs a jump if the zero flag is enabled
    {"JOZ", true, {0, 0, 0}},
    // Jump not Zero: Performs a jump if the zero flag is not enabled
    {"JNZ", true, {0, 0, 0}},
    // Display: Displays a value from the given memory address on the numeric output display
    {"DSP", true, {IO|MI, RO|OI, 0}},
    // Display Immediate: Displays the given value directly on the numeric output display
    {"DSI", true, {IO|OI, 0, 0}},
    // Display A Register: Displays the value in the A Register on the numeric output display
    {"DSA", false, {AO|OI, 0, 0}},
    // Halt: Stops the clock to halt the computer program
    {"HLT", false, {HLT, 0, 0}},
};

// List of additional instructions that can replace base instructions if the need arises.
const Instruction MISC_INSTRUCTIONS[] = {
    // Shift Left: Shifts the A Register value to the left by one bit by adding the A Register value to itself
    {"SHL", false, {AO|BI, EO|AI, 0}},
    // Negate: Negates the value in the A Register by subtracting the A Register value from 0
    {"NEG", false, {AO|BI, AI, EO|SU|AI}},
};