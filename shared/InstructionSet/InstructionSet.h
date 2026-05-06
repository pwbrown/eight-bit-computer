#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include <Arduino.h>

/*************************** CONTROL SIGNAL MASK VALUES **************************/

// Output Module (Numeric Display)
const uint16_t OUTPUT_IN = 1 << 0; // OI: Output In

// B Register
const uint16_t B_REG_IN = 1 << 1; // BI: B Register In

// Arithmetic Logic Unit (ALU)
const uint16_t FLAGS_REG_IN = 1 << 2; // FI: Flags Register In
const uint16_t SUBTRACT = 1 << 3;     // SU: Subtract Enable
const uint16_t SUM_OUT = 1 << 4;      // EO: Sum Out (ALU Out)

// A Register
const uint16_t A_REG_OUT = 1 << 5; // AO: A Register Out
const uint16_t A_REG_IN = 1 << 6;  // AI: A Register In

// Program Counter
const uint16_t JUMP = 1 << 7;      // J: Jump (Counter In)
const uint16_t COUNT_OUT = 1 << 8; // CO: Counter Out/Enable

// Auxiliary Bus Control
const uint16_t AUX_OUT = 1 << 9; // XO: Auxiliary Out (Right now this just puts a value of 1 on the bus)

// Instruction Register
const uint16_t INST_REG_OUT = 1 << 10; // IO: Instruction Register Out
const uint16_t INST_REG_IN = 1 << 11;  // II: Instruction Register In

// RAM (Memory)
const uint16_t RAM_OUT = 1 << 12;         // RO: RAM Out
const uint16_t RAM_IN = 1 << 13;          // RI: RAM In
const uint16_t MEM_ADDR_REG_IN = 1 << 14; // MI: Memory Address Register In

// MISC
const uint16_t HALT = 1 << 15; // HLT: Halt Enable (stop the clock)

/************************** Instructions *******************************/

// Enumeration of the possible instruction mnemonics for the 8-bit computer
enum InstructionType
{
    LDA, // Load A
    STA, // Store A
    LDI, // Load Immediate
    ADD, // Add
    SUB, // Subtract
    INX, // Increment With Aux
    DEX, // Decrement With Aux
    DBL, // Double (Shift Left)
    JC,  // Jump on Carry
    JNC, // Jump not Carry
    JZ,  // Jump on Zero
    JNZ, // Jump not Zero
    JMP, // Jump
    CLR, // Clear Memory (Set to 0)
    DSM, // Display Memory
    HLT, // Halt
    NOP, // No Operation (empty instruction)
};

// Definition of a single instruction
struct InstructionConfig
{
    InstructionType type; // Instruction type
    uint16_t steps[4];    // Logic values representing T2-T5 of the microcode sequence
};

// List of the first 2 microcode operations for the fetch and decode stages, which are the same for all instructions
const uint16_t FETCH_DECODE[2] = {
    // T0 - Fetch and increment program counter and load instruction into memory address register
    COUNT_OUT | MEM_ADDR_REG_IN,
    // T1 - Load instruction from memory into instruction register and preload memory address register from instruction
    RAM_OUT | INST_REG_IN | MEM_ADDR_REG_IN,
};

// The full list of ordered instructions with their control logic
const InstructionConfig INSTRUCTIONS[16] = {
    // Load A: Loads the A Register with a value from RAM at the given address
    {
        type : LDA,
        steps : {
            RAM_OUT | A_REG_IN, // Move RAM value to A Register
        },
    },

    // Store A: Stores the value in the A Register to RAM at the given address
    {
        type : STA,
        steps : {
            A_REG_OUT | RAM_IN, // Move A Register value to RAM
        },
    },

    // Load Immediate: Loads the given value directly into the A Register
    {
        type : LDI,
        steps : {
            INST_REG_OUT | A_REG_IN, // Move Instruction Argument to A Register
        },
    },

    // Add: Adds the value in RAM at the given address to the value in the A Register and stores the result in the A Register
    {
        type : ADD,
        steps : {
            RAM_OUT | B_REG_IN,                // Move RAM value to B Register
            SUM_OUT | A_REG_IN | FLAGS_REG_IN, // Move ALU Sum (A + B) into the A Register and update flags
        },
    },

    // Subtract: Subtracts the value in RAM at the given address from the value in the A Register and stores the result in the A Register
    {
        type : SUB,
        steps : {
            RAM_OUT | B_REG_IN,                           // Move RAM value to B Register
            SUM_OUT | SUBTRACT | A_REG_IN | FLAGS_REG_IN, // Move ALU Difference (A - B) into the A Register and update flags
        },
    },

    // Increment With Aux: Increments the value in RAM at the given address by the AUX value (1) and stores the result in the A Register and replaces the original memory value
    {
        type : INX,
        steps : {
            RAM_OUT | A_REG_IN,                // Move RAM value to A Register
            AUX_OUT | B_REG_IN,                // Move AUX value to B Register
            SUM_OUT | A_REG_IN | FLAGS_REG_IN, // Move ALU Sum to A Register and update flags
            A_REG_OUT | RAM_IN,                // Move A Register value to RAM
        },
    },

    // Decrement With Aux: Decrements the value in RAM at the given address by the AUX value (1) and stores the result in the A Register and replaces the original memory value
    {
        type : DEX,
        steps : {
            RAM_OUT | A_REG_IN,                           // Move RAM value to A Register
            AUX_OUT | B_REG_IN,                           // Move AUX value to B Register
            SUM_OUT | SUBTRACT | A_REG_IN | FLAGS_REG_IN, // Move ALU Difference to A Register and update flags
            A_REG_OUT | RAM_IN,                           // Move A Register value to RAM
        },
    },

    // Double (Left Shift): Doubles the value in memory at the given address and stores the result in the A Register and replaces the original memory value
    {
        type : DBL,
        steps : {
            RAM_OUT | A_REG_IN | B_REG_IN,     // Move RAM value to both A Register and B Register
            SUM_OUT | A_REG_IN | FLAGS_REG_IN, // Move ALU Sum to A Register and update flags
            A_REG_OUT | RAM_IN,                // Move A Register value to RAM
        },
    },

    // Jump on Carry: Jumps to the given memory address if the carry flag is enabled
    {
        type : JC,
        steps : {}, // Control EEPROM programmer will handle setting the steps where appropriate
    },

    // Jump not Carry: Jumps to the given memory address if the carry flag is disabled
    {
        type : JNC,
        steps : {}, // Control EEPROM programmer will handle setting the steps where appropriate
    },

    // Jump on Zero: Jumps to the given memory address if the zero flag is enabled
    {
        type : JZ,
        steps : {}, // Control EEPROM programmer will handle setting the steps where appropriate
    },

    // Jump not Zero: Jumps to the given memory address if the zero flag is disabled
    {
        type : JNZ,
        steps : {}, // Control EEPROM programmer will handle setting the steps where appropriate
    },

    // Jump: Unconditionally jumps to the given memory address
    {
        type : JMP,
        steps :
            {
                INST_REG_OUT | JUMP, // Move Instruction Argument to Program Counter to perform the jump
            },
    },

    // Clear: Clears the value in RAM at the given address by setting it to zero AND loads zero into the A Register
    {
        type : CLR,
        steps : {
            RAM_IN | A_REG_IN, // Moves the default value of 0 on the bus into RAM to clear the value and into the A Register
        },
    },

    // Display Memory: Displays the value in RAM at the given address on the numeric output display AND loads the value into the A Register
    {
        type : DSM,
        steps : {
            RAM_OUT | OUTPUT_IN | A_REG_IN, // Move RAM value to both the A Register and Output
        },
    },

    // Halt: Stops the clock to halt the program
    {
        type : HLT,
        steps : {
            HALT,
        },
    },
};

/** Returns the index of the instruction with the given type, or -1 if not found */
int8_t getInstructionIndexByType(InstructionType type)
{
    for (int8_t i = 0; i < 16; i++)
    {
        if (INSTRUCTIONS[i].type == type)
        {
            return i;
        }
    }
    return -1; // Return -1 if the instruction is not found
}

#endif // INSTRUCTION_SET_H