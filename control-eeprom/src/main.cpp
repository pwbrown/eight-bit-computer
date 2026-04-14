#include <Arduino.h>
#include "EEPROMProgrammer.h"
#include "InstructionSet.h"

// Flags register values shortcut
#define FLAGS_Z0C0 0
#define FLAGS_Z0C1 1
#define FLAGS_Z1C0 2
#define FLAGS_Z1C1 3

// Arduino setup function, runs once at startup
void setup() {
  // Start serial
  Serial.begin(57600);

  // Setup all EEPROM pins
  setupEEPROMPins();

  // Determine instruction position for each of the jump instructions
  int JMP, JOC, JNC, JOZ, JNZ = 0;
  for (int i = 0; i < 16; i += 1) {
    char* id = INSTRUCTIONS[i].id;
    if (strcmp(id, "JMP") == 0) {
      JMP = i;
    } else if (strcmp(id, "JOC") == 0) {
      JOC = i;
    } else if (strcmp(id, "JNC") == 0) {
      JNC = i;
    } else if (strcmp(id, "JOZ") == 0) {
      JOZ = i;
    } else if (strcmp(id, "JNZ") == 0) {
      JNZ = i;
    }
  }

  // Write the instructions to the EEPROM
  Serial.print("Programming EEPROM");
  for (int addr = 0; addr < 2048; addr += 1) {
    if (addr % 16 == 0) {
      Serial.print(".");
    }

    // Get bit values from the address
    int disable     = (addr & 0b10000000000) >> 10;
    int carryFlag   = (addr & 0b01000000000) >> 9;
    int zeroFlag    = (addr & 0b00100000000) >> 8;
    int byteSel     = (addr & 0b00010000000) >> 7;
    int instruction = (addr & 0b00001111000) >> 3;
    int step        = (addr & 0b00000000111);
    int instStep    = step - 2; // Instruction steps start at T2, so subtract the first two fetch/decode steps

    uint16_t value = 0;

    if (disable == 0) {
      if (step < 2) {
        // First 2 steps are always part of the fetch/decode cycle
        value = FETCH_DECODE[step];
      } else if (step < 5) {
        // Next 3 steps contain the actual instruction control logic
        value = INSTRUCTIONS[instruction].logic[instStep];
  
        // Inject jump logic if the flags register aligns with the current instruction
        if (
          // Only applies to the first instruction step (T2)
          instStep == 0 && (
            // Jump Not Zero
            (zeroFlag == 0 && instruction == JNZ) ||
            // Jump On Zero
            (zeroFlag == 1 && instruction == JOZ) ||
            // Jump Not Carry
            (carryFlag == 0 && instruction == JNC) ||
            // Jump On Carry
            (carryFlag == 1 && instruction == JOC)
          )
        ) {
          value = INSTRUCTIONS[JMP].logic[instStep];
        }
      }
  
      // Shift the value if the byte select is on
      if (byteSel == 1) {
        value = value >> 8;
      }
    }


    // Write the value to the EEPROM at the address
    writeEEPROMByte(addr, value);
  }
  Serial.println();

  delay(1000);

  printEEPROMContentsToSerial(2048);
}

void loop() {
  // put your main code here, to run repeatedly:
}