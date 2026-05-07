/**
 * Programs is a list of predefined programs written using
 * the instruction set defined in the shared InstructionSet
 * library.
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#ifndef PROGRAMS_H
#define PROGRAMS_H

#include "Program.h"

// Pre-defined programs that can be selected by the bootloader
Program PROGRAMS[] = {
    // Add 2 values together
    Program("Add (x + y)")
        .setVariables({
            {"x", 6, 2},
            {"y", 7, 2},
        })
        .setInstructions({
            {LDA, 6}, // 0: Load x value into A register
            {ADD, 7}, // 1: Add y value
            {STA, 5}, // 2: Store the sum into memory
            {DSM, 5}, // 3: Display the sum on the numeric display
            {HLT, 0}, // 4: Halt execution
        }),
    // Subtract a value from another value
    Program("Subtract (x - y)")
        .setVariables({
            {"x", 6, 4},
            {"y", 7, 2},
        })
        .setInstructions({
            {LDA, 6}, // 0: Load x value into A register
            {SUB, 7}, // 1: Subtract y value
            {STA, 5}, // 2: Store the difference into memory
            {DSM, 5}, // 3: Display the difference on the numeric display
            {HLT, 0}, // 4: Halt execution
        }),
    // Multiply 2 values together using the Shift and Add Method
    Program("Multiply (x * y)")
        .setVariables({
            {"x", 12, 2}, // Multiplicand stored at RAM address 12, defaulting to a value of 2
            {"y", 13, 2}, // Multiplier stored at RAM address 13, defaulting to a value of 2
        })
        .setInstructions({
            // LOOP: Main multiplication loop
            {DBL, 11}, // 0: Double the current product value (left shift by 1)
            // Check the next multiplier bit
            {DBL, 13}, // 1: Double the multiplier (y) value (left shift by 1)
            {JNC, 6},  // 2: Jump to the SKIP instruction if the shifted out multiplier bit (carry) was not 1
            // Otherwise if the multiplier bit was 1, add the multiplicand to the product
            {LDA, 11}, // 3: Load product into A register
            {ADD, 12}, // 4: Add multiplicand (x) to the A register value
            {STA, 11}, // 5: Store new product in memory
            // SKIP: Skip adding the multiplicand
            {DEX, 10}, // 6: Decrement the counter by 1
            {JNZ, 0},  // 7: Jump to the LOOP instruction if the new counter is not zero
            // End of the Program
            {DSM, 11}, // 8: Display the product to the numeric display
            {HLT, 0},  // 9: Halt execution
            // Values
            {NOP, 8}, // 10: counter (Always starts at 8)
            {NOP, 0}, // 11: product (Initialized to 0)
        }),
    // Divide a value by another value using repeated subtraction of the divisor
    Program("Divide (x / y)")
        .setVariables({
            {"x", 13, 4}, // Dividend
            {"y", 14, 2}, // Divisor
        })
        .setInstructions({
            // SETUP: Initialize the quotient and copy the dividend
            {CLR, 12}, // 0: Clear the quotient value
            {LDA, 13}, // 1: Load the dividend into the A register
            {STA, 11}, // 2: Store the dividend into the decrementer
            // LOOP: Main division loop
            {LDA, 11}, // 3: Load the decrementer into the A register
            {SUB, 14}, // 4: Subtract the divisor from the dividend decrementer
            {STA, 11}, // 5: Store the new decrementer value
            {JNC, 9},  // 6: Jump to the end if the decrementer is negative
            {INX, 12}, // 7: Increment the quotient if decrementer is not negative
            {JMP, 3},  // 8: Jump to the main LOOP
            // END: Print the quotient value
            {DSM, 12}, // 9: Display the quotient value on the numeric display
            {HLT, 0},  // 10: Halt execution
        }),
    // Determine the largest divisor of a number
    // Credit: https://theshamblog.com/programs-and-more-commands-for-the-ben-eater-8-bit-breadboard-computer/
    Program("Largest Divisor of x")
        .setVariables({
            {"x", 11, 16}, // The number to find the largest divisor of
        })
        .setInstructions({
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
        }),
    // Determine the smallest divisor of a number
    // Credit: https://theshamblog.com/programs-and-more-commands-for-the-ben-eater-8-bit-breadboard-computer/
    Program("Smallest Divisor of x")
        .setVariables({
            {"x", 12, 16}, // The number to find the smallest divisor of
        })
        .setInstructions({
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
            {JNC, 3},  // 7: Jump to SUB_D if the subtraction resulted in a negative number
            {JMP, 5},  // 8: Jump to SUB_D if the subtraction resulted in a positive number
            // END: Display the smallest divisor and halt execution
            {DSM, 11}, // 9: Display the divisor value
            {HLT, 0},  // 10: Halt execution
        }),
    // Count the number of "1" bits that make up a given number in binary
    Program("Count 1 Bits in x")
        .setVariables({
            {"x", 15, 255},
        })
        .setInstructions({
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
            {HLT, 0},  // 11: Halt execution
        }),
    // Count the number of "0" bits that make up a given number in binary
    Program("Count 0 Bits in x")
        .setVariables({
            {"x", 15, 0},
        })
        .setInstructions({
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
            {JC, 8},   // 6: Jump to SKIP (skip incrementing bit count) if the carried bit was not zero
            {INX, 12}, // 7: Increment the bit count (b) if the carry bit was not set
            // SKIP:
            {DEX, 13}, // 8: Decrement the loop counter (l)
            {JNZ, 5},  // 9: Jump back to LOOP if the loop counter is not zero
            // END:
            {DSM, 12}, // 10: Display the bit count value
            {HLT, 0},  // 11: Halt execution
        }),
    // Increments a value by the specified x value and displays the value on each iteration
    Program("Increment by x")
        .setVariables({
            {"x", 6, 1},
        })
        .setInstructions({
            // SETUP: Clear the value
            {CLR, 5}, // 0: Clears the display value in RAM and puts 0 in the A register
            // LOOP: Main increment loop
            {ADD, 6}, // 1: Add the increment (x) value to the A register
            {STA, 5}, // 2: Store the new display value
            {DSM, 5}, // 3: Display the current value on the numeric display
            {JMP, 1}, // 4: Start the loop again
        }),
    // Decrements a value by the specified x value and displays the value on each iteration
    Program("Decrement by x")
        .setVariables({
            {"x", 6, 1},
        })
        .setInstructions({
            // SETUP: Clear the value
            {CLR, 5}, // 0: Clears the display value in RAM and puts 0 in the A register
            // LOOP: Main decrement loop
            {SUB, 6}, // 1: Subtract the decrement (x) value to the A register
            {STA, 5}, // 2: Store the new display value
            {DSM, 5}, // 3: Display the current value on the numeric display
            {JMP, 1}, // 4: Start the loop again
        }),
};

// The total number of programs written
uint8_t NUM_PROGRAMS = sizeof(PROGRAMS) / sizeof(Program);

#endif // PROGRAMS_H