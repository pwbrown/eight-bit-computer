# Multiplication (shift and add)

## Gemini Suggestion

```assembly
   Clear Product
   Set Counter to 8
LOOP
   Shift product left
   Shift multiplier left
   Branch if not carry to SKIP
   Add Multiplicand to Product
SKIP
   Decrement the counter
   Branch if counter not zero to LOOP
   Print result
   HALT
```

## Final Assembly

```assembly
LOOP:
   DMS product; // Shift product left
   DMS y;       // Shift multiplier left
   JNC SKIP;    // Jump to skip if the carry bit is not set
   LDA product; // Load the product for adding
   ADD x;       // Add x to the product
   STA product; // Store the result
SKIP:
   LDA counter; // Load counter to A
   SUI 1;       // Decrement counter
   STA counter; // Store result
   JNZ LOOP;    // Start loop again if counter is not zero
   DSM product; // Display the product to the numeric display
   HLT;         // Stop the program

   counter = 8;
   product = 0;
   x = 16;
   y = 7;
```

## Final Binary

| #    | Binary    | ASM     |
| ---- | --------- | ------- |
| 0000 | 1110 1101 | DMS 13  |
| 0001 | 1110 1111 | DMS 15  |
| 0010 | 1001 0110 | JNC 6   |
| 0011 | 0000 1101 | LDA 13  |
| 0100 | 0011 1110 | ADD 14  |
| 0101 | 0001 1101 | STA 13  |
| 0110 | 0000 1100 | LDA 12  |
| 0111 | 0110 0001 | SUI 1   |
| 1000 | 0001 1100 | STA 12  |
| 1001 | 1011 0000 | JNZ 0   |
| 1010 | 1100 1101 | DSM 13  |
| 1011 | 1111 0000 | HLT     |
| 1100 | 0000 1000 | counter |
| 1101 | 0000 0000 | product |
| 1110 | 0001 0000 | x = 16  |
| 1111 | 0000 0111 | y = 7   |