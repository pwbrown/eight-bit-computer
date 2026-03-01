#include <Arduino.h>
#include "EEPROMProgrammer.h"

// Segment Name Indexes
const int A = 0;
const int B = 1;
const int C = 2;
const int D = 3;
const int E = 4;
const int F = 5;
const int G = 6;
const int DP = 7; // Not used in this project but included for completeness

// Sets up the on/off states for each segment for the digits 0-F in a 7-segment display (a-g)
// The possible states are 1 for on and 0 for off, and the order of the segments is a, b, c, d, e, f, g
// with the segment labels as follows (dp is for decimal point which we are not using here):
//      a
//     ---
//  f |   | b
//     -g-
//  e |   | c
//     ---  * dp
//      d
const int SEGMENT_STATES[16][8] = {
    // a, b, c, d, e, f, g, dp
    {1, 1, 1, 1, 1, 1, 0, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1, 0}, // 2
    {1, 1, 1, 1, 0, 0, 1, 0}, // 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4
    {1, 0, 1, 1, 0, 1, 1, 0}, // 5
    {1, 0, 1, 1, 1, 1, 1, 0}, // 6
    {1, 1, 1, 0, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8
    {1, 1, 1, 0, 0, 1, 1, 0}, // 9
    {1, 1, 1, 0, 1, 1, 1, 0}, // A
    {1, 0, 0, 1, 1, 1, 1, 0}, // b
    {0, 0, 1, 1, 1, 0, 1, 0}, // C
    {1, 0, 0, 1, 1, 0, 1, 0}, // d
    {1, 0, 0, 1, 1, 1, 1, 0}, // E
    {1, 0, 0, 0, 1, 1, 1, 0}  // F
};

// The order of the segment state values in order of LSB to MSB for each address of the EEPROM
const int SEGMENT_OUTPUT_ORDER[8] = {E, D, C, DP, B, A, F, G};

// Indicates if the 7-segment display is common cathode or anode,
// which determines whether a segment is turned on by setting its pin HIGH or LOW respectively
const bool COMMON_CATHODE = true; // Set to false for common anode displays

// Function declarations
byte digitToSegmentByte(int digit);

// Arduino setup function, runs once at startup
void setup()
{
  // Start serial
  Serial.begin(57600);

  // Setup all EEPROM pins
  setupEEPROMPins();

  // Initialize digits
  byte digits[10];
  for (int i = 0; i < 10; i += 1)
  {
    digits[i] = digitToSegmentByte(i);
  }

  byte blank = COMMON_CATHODE ? 0 : 0xFF; // All segments off

  int gIndex = 0;
  for (int i = 0; i < 8; i += 1) {
    if (SEGMENT_OUTPUT_ORDER[i] == G) {
      gIndex = i;
      break;
    }
  }
  byte negative = COMMON_CATHODE ? (1 << gIndex) : ~(1 << gIndex); // Only G segment active

  // Write segment byte values for unsigned values 0-255 to the EEPROM
  Serial.print("Writing unsigned segment byte values to EEPROM");
  for (int value = 0; value < 256; value += 1)
  {
    if (value % 16 == 0)
    {
      Serial.print(".");
    }
    // Write the ones place to the first 256 bytes
    int onesPlace = digits[value % 10];
    writeEEPROMByte(value, onesPlace);
    // Write the tens place to the next 256 bytes
    int tensPlace = value < 10 ? blank : digits[(value / 10) % 10];
    writeEEPROMByte(value + 256, tensPlace);
    // Write the hundreds place to the next 256 bytes
    int hundredsPlace = value < 100 ? blank : digits[(value / 100) % 10];
    writeEEPROMByte(value + 512, hundredsPlace);
    // Write blanks for the remaining 256 bytes
    writeEEPROMByte(value + 768, blank);
  }

  Serial.println();
  // Write segment byte values for twos complement values -128 to 127
  Serial.print("Writing signed (two's complement) segment byte values to EEPROM");
  for (int value = -128; value < 128; value += 1)
  {
    if (abs(value) % 16 == 0)
    {
      Serial.print(".");
    }
    // Write the ones place to the first 256 bytes
    int onesPlace = digits[abs(value) % 10];
    writeEEPROMByte(byte(value) + 1024, onesPlace);
    // Write the tens place to the next 256 bytes
    int tensPlace = (
      value > -10 && value < 0 ? negative :
      value >= 0 && value < 10 ? blank :
      digits[(abs(value) / 10) % 10]
    );
    writeEEPROMByte(byte(value) + 1280, tensPlace);
    // Write the hundreds place to the next 256 bytes
    int hundredsPlace = (
      value > -100 && value <= -10 ? negative :
      value > -10 && value < 100 ? blank :
      digits[(abs(value) / 100) % 10]
    );
    writeEEPROMByte(byte(value) + 1536, hundredsPlace);
    // Write negative sign where applicable for the remaining 256 bytes
    writeEEPROMByte(byte(value) + 1792, value <= -100 ? negative : blank);
  }
  Serial.println();

  printEEPROMContentsToSerial();
}

// Arduino loop, not used for this purpose
void loop()
{
  // Do nothing
}

// Converts a digit (0-9) to the corresponding byte value to output to the 7-segment display
byte digitToSegmentByte(int digit)
{
  // Validate input
  if (digit < 0 || digit > 9)
  {
    return 0;
  }
  byte segmentByte = 0;
  for (int i = 7; i >= 0; i -= 1)
  {
    int segmentState = SEGMENT_STATES[digit][SEGMENT_OUTPUT_ORDER[i]];
    if (!COMMON_CATHODE)
    {
      segmentState = 1 - segmentState; // Invert state for common anode
    }
    segmentByte = (segmentByte << 1) | segmentState;
  }
  return segmentByte;
}