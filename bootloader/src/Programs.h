#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <Arduino.h>
#include "Program.h"
#include "InstructionSet.h"

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
            {HLT},       // 9: Halt execution
            // Values
            {8},         // 10: counter (Always starts at 8)
            {0},         // 11: product (Initialized to 0)
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
            {DSM, 11}, // 8: Display the divisor value
            {HLT},     // 9: Halt execution
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
            {HLT},     // 10: Halt execution
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
            {HLT},     // 11: Halt execution
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
            if (variable.name != nullptr && variable.address == instruction.arg) {
                instructionByte = byte(variable.value);
                break;
            }
        }
    } else {
        // Validate arg value to be between 0 and 15 since we only have 4 bits to work with and no access to the sign bit
        if (instruction.arg < 0 || instruction.arg > 15) {
            printErrorMessage("getProgramInstructionByte", "Instruction argument value must be between 0 and 15");
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
void setProgramVariable(uint8_t programIndex, uint8_t variableIndex, int8_t value) {
    if (
        !isValidProgramIndex(programIndex, "setProgramVariable") ||
        !isValidInstructionIndex(variableIndex, "setProgramVariable")
    ) {
        return;
    }
    // Validate variable index for the specific program
    if (PROGRAMS[programIndex].variables[variableIndex].name == nullptr) {
        printErrorMessage("setProgramVariable", "Cannot set a program variable that does not exist");
        return;
    }
    // Validate the value
    if (value < -128 || value > 255) {
        printErrorMessage("setProgramVariable", "Program variable value must be a valid signed or unsigned 8 bit value");
        return;
    }
    // Set the value and return
    PROGRAMS[programIndex].variables[variableIndex].value = value;
}

// Validate the program index
bool isValidProgramIndex(uint8_t programIndex, const char* errorContext) {
    if (programIndex >= NUM_PROGRAMS) {
        printErrorMessage(errorContext, "Invalid program index");
        return false;
    }
    return true;
}

// Validate the program instruction index
bool isValidInstructionIndex(uint8_t instructionIndex, const char* errorContext) {
    if (instructionIndex >= MAX_PROGRAM_INSTRUCTIONS) {
        printErrorMessage(errorContext, "Invalid instruction index");
        return false;
    }
    return true;
}

// Prints an error message to the serial console with the error context
void printErrorMessage(const char* errorContext, const char* errorMessage) {
    Serial.print("ERROR (");
    Serial.print(errorContext);
    Serial.print("): ");
    Serial.println(errorMessage);
}

#endif // PROGRAMS_H
