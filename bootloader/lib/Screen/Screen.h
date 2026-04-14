#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define BUTTON_BOX_WIDTH 50
#define BUTTON_BOX_PADDING 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Function declarations
void displayButtonLabels(const char*, const char*, const char*, const char*);


/**
 * Initializes the SSD1306 screen and clears the display.
 */
void initializeScreen() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

/**
 * Displays the labels for the four corner buttons.
 * Puts a box around each button label with the box width equal to the longest label.
 */
void displayButtonLabels(const char* tl, const char* tr, const char* bl, const char* br) {
    int16_t x, y;
    uint16_t w, h;
    uint16_t maxWidth = 0;
    
    // Top left button text
    if (tl != nullptr) {
        display.getTextBounds(tl, 0, 0, &x, &y, &w, &h);
        if (w > maxWidth) maxWidth = w;
        display.setCursor(0, 0);
        display.print(tl);
    }

    // Top right button text
    if (tr != nullptr) {
        display.getTextBounds(tr, 0, 0, &x, &y, &w, &h);
        if (w > maxWidth) maxWidth = w;
        display.setCursor(SCREEN_WIDTH - w, 0);
        display.print(tr);
    }

    // Bottom left button text
    if (bl != nullptr) {
        display.getTextBounds(bl, 0, 0, &x, &y, &w, &h);
        if (w > maxWidth) maxWidth = w;
        display.setCursor(0, SCREEN_HEIGHT - h);
        display.print(bl);
    }

    // Bottom right button text
    if (br != nullptr) {
        display.getTextBounds(br, 0, 0, &x, &y, &w, &h);
        if (w > maxWidth) maxWidth = w;
        display.setCursor(SCREEN_WIDTH - w, SCREEN_HEIGHT - h);
        display.print(br);
    }

    // Top left button box
    if (tl != nullptr) {
        display.drawLine(maxWidth + BUTTON_BOX_PADDING, 0, maxWidth + BUTTON_BOX_PADDING, h + BUTTON_BOX_PADDING, SSD1306_WHITE);
        display.drawLine(0, h + BUTTON_BOX_PADDING, maxWidth + BUTTON_BOX_PADDING, h + BUTTON_BOX_PADDING, SSD1306_WHITE);
    }

    // Top right button box
    if (tr != nullptr) {
        display.drawLine(SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, 0, SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, h + BUTTON_BOX_PADDING, SSD1306_WHITE);
        display.drawLine(SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, h + BUTTON_BOX_PADDING, SCREEN_WIDTH, h + BUTTON_BOX_PADDING, SSD1306_WHITE);
    }

    // Bottom left button box
    if (bl != nullptr) {
        display.drawLine(0, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, maxWidth + BUTTON_BOX_PADDING, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, SSD1306_WHITE);
        display.drawLine(maxWidth + BUTTON_BOX_PADDING, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, maxWidth + BUTTON_BOX_PADDING, SCREEN_HEIGHT, SSD1306_WHITE);
    }

    // Bottom right button box
    if (br != nullptr) {
        display.drawLine(SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, SCREEN_HEIGHT, SSD1306_WHITE);
        display.drawLine(SCREEN_WIDTH - maxWidth - BUTTON_BOX_PADDING, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, SCREEN_WIDTH, SCREEN_HEIGHT - h - BUTTON_BOX_PADDING, SSD1306_WHITE);
    }

    display.display();
}