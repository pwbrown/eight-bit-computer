#ifndef SETTINGS_H
#define SETTINGS_H

// All pin definitions for the project
#define SHIFT_DATA_PIN 2  // Data pin for the shift register chain
#define SHIFT_LATCH_PIN 3 // Latch pin for all shift registers
#define SHIFT_CLOCK_PIN 4 // Clock pin for all shift registers
#define DATA_ENABLE_PIN 5 // Connected to both data shift registers to enable/disable data output
#define CTRL_ENABLE_PIN 6 // Connected to the control shift register to enable/disable control signal output
#define RAM_MODE_PIN 7    // Connected to the RAM mode switch on the computer to detect current mode ("Run" vs "Program")
#define TR_BUTTON_PIN 8   // Connected to the top right button next to the screen
#define BR_BUTTON_PIN 9   // Connected to the bottom right button next to the screen
#define CLOCK_MODE_PIN 10 // Connected to the clock mode switch on the computer to detect current clock mode ("Auto" vs "Manual")
#define TL_BUTTON_PIN 11  // Connected to the top left button next to the screen
#define BL_BUTTON_PIN 12  // Connected to the bottom left button next to the screen

// Control shift register bit definitions
#define CTRL__CTRL_WORD_ADDR_BIT 0       // Connected to the control word EEPROM(s) last address bit (A10)
#define CTRL__RESET_BUTTON_BIT 1         // Connected to the computer's reset button
#define CTRL__RAM_WRITE_MUX_STROBE_BIT 2 // Connected to the RAM write multiplexer's strobe signal pin
#define CTRL__RAM_ADDR_REG_ENABLE_BIT 3  // Connected to the RAM address register's output enable pin

// Screen settings
#define SCREEN_ADDRESS 0x3C // I2C address of the OLED screen

// Behavior settings
#define BUTTON_HOLD_THRESHOLD_MS 500 // Time in milliseconds to consider a button as "held"

#endif // SETTINGS_H