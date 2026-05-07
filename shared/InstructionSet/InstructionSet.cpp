#include "InstructionSet.h"

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