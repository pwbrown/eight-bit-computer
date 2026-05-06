#include "Program.h"

/********************** VARIABLE **********************/

const char *VARIABLE_FORMAT = "%s = %d";
const char *VARIABLE_SEPERATOR = ", ";
const uint8_t VARIABLE_SEPERATOR_LENGTH = strlen(VARIABLE_SEPERATOR);

// Gets the length of the string representation of a variable
int getVariableStringLength(const char *name, int16_t value)
{
    return snprintf(NULL, 0, VARIABLE_FORMAT, name, value);
}

// Returns the string representation of the variable in the format "name = value"
char *Program::Variable::toString()
{
    // Initialize string buffer with the appropriate length
    int length = getVariableStringLength(name, value);
    char *buf = (char *)malloc(length + 1); // +1 for null terminator

    // Fill the string buffer
    sprintf(buf, VARIABLE_FORMAT, name, value);
    buf[length] = '\0'; // Null terminate the string

    return buf;
}

/************************* PROGRAM **********************/

// Returns the program name
const char *Program::getName()
{
    return name;
}

// Set program variables from a provided list of variables
Program &Program::setVariables(const Variable (&vars)[MAX_PROGRAM_VARIABLES])
{
    for (int i = 0; i < MAX_PROGRAM_VARIABLES; i++)
    {
        // Stop once we hit an empty variable
        if (vars[i].name == nullptr)
        {
            break; // Assume variables are defined in order
        }
        // Validate the address
        if (vars[i].address < 0 || vars[i].address >= MAX_PROGRAM_INSTRUCTIONS)
        {
            // Error for invalid address
            break;
        }
        // Validate the value
        if (vars[i].value < -128 || vars[i].value > 255)
        {
            // Error for invalid variable value
            break;
        }
        variables[i] = vars[i];
        variableCount += 1;
    }
    return *this;
}

// Set the program instructions from a provided list of instructions
Program &Program::setInstructions(const Instruction (&insts)[MAX_PROGRAM_INSTRUCTIONS])
{
    for (int i = 0; i < MAX_PROGRAM_INSTRUCTIONS; i++)
    {
        instructions[i] = insts[i];
    }
    return *this;
}

// Gets the value for a variable at the given index (or returns 0 if the index is invalid)
int16_t Program::getVariableValue(uint8_t variableIndex)
{
    if (variableIndex >= variableCount)
    {
        // Error: invalid variable index
        return 0;
    }
    return variables[variableIndex].value;
}

// Gets the string representation for a variable at the given index (or returns nullptr if the index is invalid)
char *Program::getVariableString(uint8_t variableIndex)
{
    if (variableIndex >= variableCount)
    {
        // Error: invalid variable index
        return nullptr;
    }
    return variables[variableIndex].toString();
}

// Sets the value for a variable at the given index (does nothing if the index is invalid)
void Program::setVariableValue(uint8_t variableIndex, int16_t value)
{
    if (variableIndex >= variableCount)
    {
        // Error: invalid variable index
        return;
    }
    variables[variableIndex].value = value;
}

// Indicates if the variable at the given index should be treated as a half byte value (returns false if the index is invalid)
bool Program::isVariableHalfByte(uint8_t variableIndex)
{
    if (variableIndex >= variableCount)
    {
        // Error: invalid variable index
        return false;
    }
    // If the instruction at the variable address is not a NOP, then the value should be limited to a half byte
    if (instructions[variables[variableIndex].address].type != NOP)
    {
        return true;
    }
    return false;
}

// Returns the byte representation of the instruction at the given index with variable substitution for NOP instructions (returns 0 if the instruction index is invalid)
byte Program::getInstructionByte(uint8_t instructionIndex)
{
    if (instructionIndex >= MAX_PROGRAM_INSTRUCTIONS)
    {
        // Error: invalid instruction index
        return 0;
    }

    Instruction instruction = instructions[instructionIndex];
    byte instructionByte = 0;
    bool halfByteSubstitution = false;

    // Handle NOP instructions by using the argument variable and handle variable substitution
    if (instruction.type == NOP)
    {
        instructionByte = byte(instruction.argument);
    }
    else
    {
        // Validate the argument value to be between 0 and 15 since we only have 4 bits that do not include the sign bit
        if (instruction.argument < 0 || instruction.argument > 15)
        {
            // Error: invalid instruction argument value
            return 0;
        }

        // Apply the index of the instruction by type to the most significant 4 bits
        instructionByte |= getInstructionIndexByType(instruction.type) << 4;
        // Apply the argument to the least significant 4 bits
        instructionByte |= instruction.argument; // No mask needed since we have already validated the argument value

        // Indicate that variable substitution should not override the most significant 4 bits which represent the instruction type
        halfByteSubstitution = true;
    }

    // Handle variable substitution based on the instruction address
    for (int i = 0; i < variableCount; i++)
    {
        Variable variable = variables[i];
        if (variable.address == instructionIndex)
        {
            instructionByte |= byte(halfByteSubstitution ? (variable.value & 0x0F) : variable.value);
            break;
        }
    }

    return instructionByte;
}

// Returns a string representation of all variables in the format "name1 = value1[, name2 = value2, ...]"
char *Program::getVariablesString()
{
    // Track the total expected length of the final string
    int totalLength = 0;
    // Append variable string lengths
    for (int i = 0; i < variableCount; i++)
    {
        totalLength += getVariableStringLength(variables[i].name, variables[i].value);
    }
    // Append the seperator lengths
    if (variableCount > 1)
    {
        totalLength += VARIABLE_SEPERATOR_LENGTH * (variableCount - 1);
    }
    // Initialize the string buffer
    char *buf = (char *)malloc(totalLength + 1); // +1 for null terminator
    buf[0] = '\0';                               // Initialize the buffer as an empty string (otherwise strcat will break)
    // Append each variable to the buffer
    for (int i = 0; i < variableCount; i++)
    {
        // Append the seperator if not the first variable
        if (i > 0)
        {
            strcat(buf, VARIABLE_SEPERATOR);
        }
        // Generate, append, and then destroy the variable string
        char *varString = variables[i].toString();
        strcat(buf, varString);
        free(varString);
    }
    // Double check that the buffer is null terminated
    buf[totalLength] = '\0';
    return buf;
}

// Returns the current variable count for the program
uint8_t Program::getVariableCount()
{
    return variableCount;
}