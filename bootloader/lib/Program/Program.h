#ifndef PROGRAM_H
#define PROGRAM_H

#include <Arduino.h>
#include "InstructionSet.h"

const int MAX_PROGRAM_INSTRUCTIONS = 16; // Maximum number of instructions a program can have
const int MAX_PROGRAM_VARIABLES = 8;     // Maximum number of variables a program can have

// Data structure for a single program variable that can be modified by the user before programming into RAM
struct ProgramVariable {
    ProgramVariable() : name(nullptr), address(0), value(0) {}
    ProgramVariable(const char* name, uint8_t address, int8_t value = 0) : name(name), address(address), value(value) {}
    const char* name; // Human readable name for the variable
    uint8_t address;  // RAM address to override
    int8_t value;        // The current variable value
};

// Data structure for a single program instruction
struct ProgramInstruction {
    ProgramInstruction() : inst(NOP), arg(0) {} // Default to NOP for empty instructions
    ProgramInstruction(Instruction inst, int8_t arg = 0) : inst(inst), arg(arg) {}
    ProgramInstruction(int8_t arg) : inst(NOP), arg(arg) {} // For instructions that are just a numeric value
    Instruction inst; // The instruction type
    int8_t arg;       // The instruction argument (if applicable)
};

// The data structure for a full program with
struct Program {
    const char* name;                                          // Human readable program name
    bool destructive;                                          // Whether the program is destructive and will alter RAM values that require reprogramming after each use
    ProgramVariable variables[MAX_PROGRAM_VARIABLES];          // List of program variables
    ProgramInstruction instructions[MAX_PROGRAM_INSTRUCTIONS]; // List of program instructions
};

#endif // PROGRAM_H