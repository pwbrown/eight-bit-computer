#ifndef SCREEN_H
#define SCREEN_H

#include <Types.h> // Contains the Corner enum definition

using namespace Types;

namespace Screen
{
    void setup(); // Setup the screen hardware
    void clear(); // Clear the screen contents

    void drawCornerLabel(Corner corner, const char *label);
    void drawMiddleLabel(const char *label, bool top = true);
}

#endif // SCREEN_H