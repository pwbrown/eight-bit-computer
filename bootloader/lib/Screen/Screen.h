#ifndef SCREEN_H
#define SCREEN_H

#include <SharedTypes.h> // Contains the Corner enum definition

using namespace SharedTypes;

namespace Screen {
    void setup();  // Setup the screen hardware
    void clear();  // Clear the screen contents

    void drawCornerLabel(Corner corner, const char* label);
    void drawMiddleLabel(const char* label);
}

#endif // SCREEN_H