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
    uint16_t logic[4]; // Logic values representing T2-T5 of the microcode sequence
};

// List of the first 2 microcode operations for the fetch and decode stages, which are the same for all instructions
const uint16_t FETCH_DECODE[2] = {
    // T0 - Fetch and increment program counter and load instruction into memory address register
    CO|CE|MI,
    // T1 - Load instruction from memory into instruction register and preload memory address register from instruction
    RO|II|MI,
};

// The full list of ordered instructions with their control logic (template)
const Instruction INSTRUCTIONS[16] = {
    // 0000: Load A: Loads the A Register with a value from RAM at the given address
    {"LDA", {RO|AI, 0, 0, 0}},
    // 0001: Store A: Stores the value in the A Register into RAM at the given address
    {"STA", {AO|RI, 0, 0, 0}},
    // 0010: Load Immediate: Loads the given value directly into the A Register
    {"LDI", {IO|AI, 0, 0, 0}},
    // 0011: Add: Adds the value at the given memory address to the A Register and stores the result in the A Register
    {"ADD", {RO|BI, EO|AI|FI, 0, 0}},
    // 0100: Subtract: Subtracts the value at the given memory address from the A Register and stores the result in the A Register
    {"SUB", {RO|BI, EO|SU|AI|FI, 0, 0}},
    // 0101: Increment: Increments the value in memory at the given address by 1 and stores the result back in memory
    {"INC", {RO|AI, BI, EO|AI|FI, AO|RI}},
    // 0110: Decrement: Decrements the value in memory at the given address by 1 and stores the result back in memory
    {"DEC", {RO|AI, BI, EO|SU|AI|FI, AO|RI}},
    // 0111: Shift Left: Shifts the value in memory at the given address left by 1 and stores the result back in memory
    {"SHL", {RO|AI|BI, EO|AI|FI, AO|RI, 0}},
    // 1000: Jump on Carry: Performs a jump if the carry flag is enabled
    {"JC", {0, 0, 0, 0}},
    // 1001: Jump not Carry: Performs a jump if the carry flag is not enabled
    {"JNC", {0, 0, 0, 0}},
    // 1010: Jump on Zero: Performs a jump if the zero flag is enabled
    {"JZ", {0, 0, 0, 0}},
    // 1011: Jump not Zero: Performs a jump if the zero flag is not enabled
    {"JNZ", {0, 0, 0, 0}},
    // 1100: Jump: Performs an unconditional jump to the given instruction address
    {"JMP", {IO|J, 0, 0, 0}},
    // 1101: Output: Displays the value in the A Register on the numeric output display
    {"OUT", {AO|OI, 0, 0, 0}},
    // 1110: Display Memory: Displays the value in memory at the given address on the numeric output display
    {"DSM", {RO|OI, 0, 0, 0}},
    // 1111: Halt: Stops the clock to halt the computer program
    {"HLT", {HLT, 0, 0, 0}},
};
