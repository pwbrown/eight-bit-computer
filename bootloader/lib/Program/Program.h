#ifndef PROGRAM_H
#define PROGRAM_H

#include <Arduino.h>
#include "InstructionSet.h"

// Constants
const uint8_t MAX_PROGRAM_INSTRUCTIONS = 16; // Maximum number of instructions in a program (RAM size in bytes)
const uint8_t MAX_PROGRAM_VARIABLES = 4;     // Maximum number of variables a program can have (helps with pre-allocation )

// Program definition
class Program
{
public:
    // Variable definition
    struct Variable
    {
        Variable() : name(nullptr), address(0), value(0) {}
        Variable(const char *name, uint8_t address, int16_t value = 0) : name(name), address(address), value(value) {}

        const char *name; // Human readable variable name
        uint8_t address;  // Address in RAM where the variable resides
        int16_t value;    // Variable value between -128 and 255 (8 bits signed or unsigned)

        // Return the string representation of the variable in the format "name = value"
        char *toString();
    };

    // Instruction definition
    struct Instruction
    {
        InstructionType type; // The instruction type
        int16_t argument;     // The instruction argument (if applicable)
    };

    // Initialize program with variables and instructions
    Program(const char *name) : name(name), variables(), instructions() {} // Default constructor with empty name, variables, and instructions

    // Returns the program name
    const char *getName();

    // Sets the list of variables for the program
    Program &setVariables(const Variable (&vars)[MAX_PROGRAM_VARIABLES]);

    // Sets the list of instructions for the program
    Program &setInstructions(const Instruction (&insts)[MAX_PROGRAM_INSTRUCTIONS]);

    // Gets the current value for a program variable at the given index
    int16_t getVariableValue(uint8_t variableIndex);

    // Returns a string representation for a variable at the given index
    char *getVariableString(uint8_t variableIndex);

    // Sets a new value for a program variable at the given index
    void setVariableValue(uint8_t variableIndex, int16_t value);

    // Indicates if the variable at the given index should be treated as a half byte value
    bool isVariableHalfByte(uint8_t variableIndex);

    // Gets the byte representation of the instruction at the given index with variable substitution
    byte getInstructionByte(uint8_t instructionIndex);

    // Returns a string representation of all variables in the format "name1 = value1[, name2 = value2, ...]"
    char *getVariablesString();

    uint8_t getVariableCount();

private:
    const char *name;                                   // Human readable program name
    Variable variables[MAX_PROGRAM_VARIABLES];          // List of program variables
    uint8_t variableCount = 0;                          // The number of variables being used
    Instruction instructions[MAX_PROGRAM_INSTRUCTIONS]; // List of program instructions
};

#endif // PROGRAM_H