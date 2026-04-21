#ifndef PROGRAMS_H
#define PROGRAMS_H

struct Program {
    const char* name;
    const char* assembly;
};

Program PROGRAMS[] = {
    {
        name: "Multiply: x * y",
        assembly: R"(
            // Variables
            counter = 8
            product = 0
            x = 2        // Default value
            y = 2        // Default value
            
            // Main multiplication loop
            LOOP:
                // Shift the product before adding
                SHL product
                
                // Check the next multiplier bit
                SHL y        // Left shift the multiplier
                JNC SKIP     // Jump to SKIP if the shifted out multipler bit was zero

                // Add the multiplicand to the product (if the multipler bit was 1)
                LDA product  // Load product into A register
                ADD x        // Add multiplicand to the A register value
                STA product  // Store new product in memory

            // Skip adding the multiplicand
            SKIP:
                DEC counter  // Decrement the counter by 1
                JNZ LOOP     // Jump to LOOP if the new counter is not zero
            
            // End of the Program
                DSM product  // Display the product to the numeric display
                HLT          // Halt execution
        )"
    }
};


#endif // PROGRAMS_H