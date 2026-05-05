#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <Arduino.h>
#include "InstructionSet.h"

const uint8_t MAX_PROGRAM_INSTRUCTIONS = 16; // Maximum number of instructions a program can have
const uint8_t MAX_PROGRAM_VARIABLES = 8;     // Maximum number of variables a program can have

const char* VARIABLE_ASSIGN = " = ";
const int VARIABLE_ASSIGN_LENGTH = strlen(VARIABLE_ASSIGN);
const char* VARIABLE_SEPERATOR = ", ";
const int VARIABLE_SEPERATOR_LENGTH = strlen(VARIABLE_SEPERATOR);

// Function declarations
bool isValidProgramIndex(uint8_t programIndex, const char* errorContext);
bool isValidInstructionIndex(uint8_t instructionIndex, const char* errorContext);
int getVariablePrintLength(const char* name, int16_t value);

// Data structure for a single program variable that can be modified by the user before programming into RAM
struct ProgramVariable {
    ProgramVariable() : name(nullptr), address(0), value(0) {}
    ProgramVariable(const char* name, uint8_t address, int16_t value = 0) : name(name), address(address), value(value) {}
    const char* name; // Human readable name for the variable
    uint8_t address;  // RAM address to override
    int16_t value;    // The current variable value

    // Returns a string representation of the variable in the format "name = value" (must be deallocated by caller)
    char* print() {
        int length = getVariablePrintLength(name, value);
        char* buf = (char*)malloc(length + 1); // +1 for null terminator
        sprintf(buf, "%s%s%d", name, VARIABLE_ASSIGN, value);
        buf[length] = '\0'; // Null terminate the string
        return buf;
    }
};

// Data structure for a single program instruction
struct ProgramInstruction {
    ProgramInstruction() : inst(NOP), arg(0) {} // Default to NOP for empty instructions
    ProgramInstruction(Instruction inst, int8_t arg = 0) : inst(inst), arg(arg) {}
    Instruction inst; // The instruction type
    int8_t arg;       // The instruction argument (if applicable)
};

// The data structure for a full program with
struct Program {
    const char* name;                                          // Human readable program name
    bool destructive;                                          // Whether the program is destructive and will alter RAM values that require reprogramming after each use
    ProgramVariable variables[MAX_PROGRAM_VARIABLES];          // List of program variables
    ProgramInstruction instructions[MAX_PROGRAM_INSTRUCTIONS]; // List of program instructions

    /** Returns a string representation of all variables in the format "name = value[, name = value]" */
    char* printVariables() {
        int usedVariables = 0;
        int totalLength = 0;
        for (int i = 0; i < MAX_PROGRAM_VARIABLES; i += 1) {
            if (variables[i].name != nullptr) {
                usedVariables += 1;
                totalLength += getVariablePrintLength(variables[i].name, variables[i].value);
            } else {
                break; // Assume variables are defined in order
            }
        }
        /** Append the seperator(s) to the total length */
        if (usedVariables > 1) {
            totalLength += VARIABLE_SEPERATOR_LENGTH * (usedVariables - 1);
        }
        char* buf = (char*)malloc(totalLength + 1);
        buf[0] = '\0'; // Initialize the buffer as an empty string
        /** Append the variables to the buffer */
        for (int i = 0; i < usedVariables; i += 1) {
            /** Append the seperator if not the first variable */
            if (i > 0) {
                strcat(buf, VARIABLE_SEPERATOR);
            }
            /** Generate, append, and then destroy the variable string */
            char* variableString = variables[i].print();
            strcat(buf, variableString);
            free(variableString);
        }
        buf[totalLength] = '\0'; // Null terminate the string
        return buf;
    }
};

// List of all the programs that can be loaded into RAM and executed by the computer
Program PROGRAMS[] = {
    {
        name: "Multiplication",
        destructive: true, // This program is destructive and must be reprogrammed ater each use to reset the counter, product, and multiplier values in RAM
        variables: {
            {"x", 12, 2}, // Multiplicand stored at RAM address 12, defaulting to a value of 2
            {"y", 13, 2}, // Multiplier stored at RAM address 13, defaulting to a value of 2
        },
        instructions: {
            // LOOP: Main multiplication loop
            {DBL, 11},   // 0: Double the current product value (left shift by 1)
            // Check the next multiplier bit
            {DBL, 13},   // 1: Double the multiplier (y) value (left shift by 1)
            {JNC, 6},    // 2: Jump to the SKIP instruction if the shifted out multiplier bit (carry) was not 1
            // Otherwise if the multiplier bit was 1, add the multiplicand to the product
            {LDA, 11},   // 3: Load product into A register
            {ADD, 12},   // 4: Add multiplicand (x) to the A register value
            {STA, 11},   // 5: Store new product in memory
            // SKIP: Skip adding the multiplicand
            {DEX, 10},   // 6: Decrement the counter by 1
            {JNZ, 0},    // 7: Jump to the LOOP instruction if the new counter is not zero
            // End of the Program
            {DSM, 11},   // 8: Display the product to the numeric display
            {HLT, 0},    // 9: Halt execution
            // Values
            {NOP, 8},    // 10: counter (Always starts at 8)
            {NOP, 0},    // 11: product (Initialized to 0)
        },
    },
    {
        // Credit: https://theshamblog.com/programs-and-more-commands-for-the-ben-eater-8-bit-breadboard-computer/
        name: "Largest Divisor",
        destructive: false, // This program is not destructive and can be executed multiple times using the hardware reset button
        variables: {
            {"x", 11, 16}, // The number to find the largest divisor of
        },
        instructions: {
            // SETUP: Display the number and copy it to another memory address as the divisor
            {DSM, 11}, // 0: Displays the x value from memory AND loads it into the A Register
            {STA, 10}, // 1: Copy the x value to memory as the divisor (d)
            // DEC_D: Decrement the divisor to test the next number
            {DEX, 10}, // 2: Decrement the divisor in memory (d) by 1
            {LDA, 11}, // 3: Load x value from memory into the A Register
            // SUB_D: Subtract the divisor from the current A register value
            {SUB, 10}, // 4: Subtract the divisor (d) from the A Register
            {JZ, 8},   // 5: Jump to the END if the subtraction resulted in zero
            {JNC, 2},  // 6: Jump to DEC_D if the subtraction resulted in a negative number
            {JMP, 4},  // 7: Jump to SUB_D if the subtraction resulted in a positive number
            // END: Display the largest divisor and halt execution
            {DSM, 10}, // 8: Display the divisor value
            {HLT, 0},  // 9: Halt execution
        },
    },
    {
        // Credit: https://theshamblog.com/programs-and-more-commands-for-the-ben-eater-8-bit-breadboard-computer/
        name: "Smallest Divisor",
        destructive: false, // This program is not destructive and can be executed multiple times using the hardware reset button
        variables: {
            {"x", 12, 16}, // The number to find the smallest divisor of
        },
        instructions: {
            // SETUP: Display the number and load a 1 into the divisor
            {DSM, 12}, // 0: Display x value
            {LDI, 1},  // 1: Load a 1 into the A register
            {STA, 11}, // 2: Store the 1 into memory as the divisor (d)
            // INC_D: Increment the divisor to test the next number
            {INX, 11}, // 3: Increment the divisor in memory (d) by 1
            {LDA, 12}, // 4: Load x value from memory into the A Register
            // SUB_D: Subtract the divisor from the current A register value
            {SUB, 11}, // 5: Subtract the divisor (d) from the A Register
            {JZ, 9},   // 6: Jump to the END if the subtraction resulted in zero
            {JNC, 3},  // 7: Jump to INC_D if the subtraction resulted in a negative number
            {JMP, 5},  // 8: Jump to SUB_D if the subtraction resulted in a positive number
            // END: Display the smallest divisor and halt execution
            {DSM, 11}, // 9: Display the divisor value
            {HLT, 0},     // 10: Halt execution
        },
    },
    {
        name: "Count Bits",
        destructive: false,
        variables: {
            {"x", 15, 255},
        },
        instructions: {
            // SETUP:
            // Copy the input number (x) into another memory spot for manipulation
            {LDA, 15}, // 0: Load the input number into A register
            {STA, 14}, // 1: Store A register in memory as a copy of x (xc)
            // Set the loop counter to 8
            {LDI, 8},  // 2: Load 8 into the A Register
            {STA, 13}, // 3: Store A register in memory as the loop counter (l)
            // Clear the bit count value in memory to prepare for counting
            {CLR, 12}, // 4: Clears the bit count value (set to zero)
            // LOOP:
            // Double the copied value to shift one of its bits out to be evaluated
            {DBL, 14}, // 5: Double (left shift) the copied value (xc)
            {JNC, 8},  // 6: Jump to SKIP (skip incrementing bit count) if the carried bit was zero
            {INX, 12}, // 7: Increment the bit count (b) if the carry bit was set
            // SKIP:
            {DEX, 13}, // 8: Decrement the loop counter (l)
            {JNZ, 5},  // 9: Jump back to LOOP if the loop counter is not zero
            // END:
            {DSM, 12}, // 10: Display the bit count value
            {HLT, 0},     // 11: Halt execution
        }
    }
};

// Total number of programs in the PROGRAMS list
int NUM_PROGRAMS = sizeof(PROGRAMS) / sizeof(Program);

// Get the byte value for a specific instruction in a program accounting for variable substitution
byte getProgramInstructionByte(uint8_t programIndex, uint8_t instructionIndex) {
    if (
        !isValidProgramIndex(programIndex, "getProgramInstructionByte") ||
        !isValidInstructionIndex(instructionIndex, "getProgramInstructionByte")
    ) {
        return 0;
    }
    ProgramInstruction instruction = PROGRAMS[programIndex].instructions[instructionIndex];

    byte instructionByte = 0;

    // Handle NOP instructions by simply using the argument value
    if (instruction.inst == NOP) {
        instructionByte = byte(instruction.arg);

        // Handle variable substitution
        for (int i = 0; i < MAX_PROGRAM_VARIABLES; i += 1) {
            ProgramVariable variable = PROGRAMS[programIndex].variables[i];
            if (variable.name != nullptr && variable.address == instructionIndex) {
                instructionByte = byte(variable.value);
                break;
            }
        }
    } else {
        // Validate arg value to be between 0 and 15 since we only have 4 bits to work with and no access to the sign bit
        if (instruction.arg < 0 || instruction.arg > 15) {
            // Error: invalid instruction argument value
            return 0;
        }
        
        // For regular instructions, apply the instruction index to the most significant 4 bits and apply a postivie argument to the least significant 4 bits
        instructionByte |= getInstructionIndexById(instruction.inst) << 4;
        instructionByte |= instruction.arg; // No need to mask since we have already validated above

        // No variable substitution is allowed for regular instructions
    }

    return instructionByte;
}

// Updates the value of a program variable
void setProgramVariable(uint8_t programIndex, uint8_t variableIndex, int16_t value) {
    if (
        !isValidProgramIndex(programIndex, "setProgramVariable") ||
        !isValidInstructionIndex(variableIndex, "setProgramVariable")
    ) {
        return;
    }
    // Validate variable index for the specific program
    if (PROGRAMS[programIndex].variables[variableIndex].name == nullptr) {
        // Error: cannot set a program variable that does not exist
        return;
    }
    // Validate the value
    if (value < -128 || value > 255) {
        // Error: invalid program variable value
        return;
    }
    // Set the value and return
    PROGRAMS[programIndex].variables[variableIndex].value = value;
}

// Validate the program index
bool isValidProgramIndex(uint8_t programIndex, const char* errorContext) {
    if (programIndex >= NUM_PROGRAMS) {
        // Error: invalid program index
        return false;
    }
    return true;
}

// Validate the program instruction index
bool isValidInstructionIndex(uint8_t instructionIndex, const char* errorContext) {
    if (instructionIndex >= MAX_PROGRAM_INSTRUCTIONS) {
        // Error: invalid instruction index
        return false;
    }
    return true;
}

// Return the length of a variable's print string
int getVariablePrintLength(const char* name, int16_t value) {
    return snprintf(NULL, 0, "%s%s%d", name, VARIABLE_ASSIGN, value); // Get the length of the string that would be printed for the variable
}

#endif // PROGRAMS_H
