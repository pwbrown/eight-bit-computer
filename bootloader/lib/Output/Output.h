#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>

// "Static" class the manage the outputs for the bootloader
class Output {
    public:
        // Initilize output shift register pins and enable pins
        static void setup();
        
        // Update the shift registers with the current data and control values
        static void update();

        // Toggle the output enable for the data shift registers (RAM address and value)
        static void enableDataOutput();
        static void disableDataOutput();

        // Toggle the output enable for the control shift register (control signals)
        static void enableControlOutput();
        static void disableControlOutput();

        // Update data register values
        static void setRamAddress(uint8_t address);
        static void setRamValue(uint8_t value);

        // Update control register values
        static void setResetButton(bool pressed);
        static void setRamWrite(bool enabled);

    private:
        static uint8_t ramAddress;
        static uint8_t ramValue;
        static bool pressingResetButton;
        static bool enablingRamWrite;
};

#endif // OUTPUT_H