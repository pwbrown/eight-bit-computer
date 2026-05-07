/**
 * The Types namespace contains shared types that can be used by the bootloader
 * and its related libraries.
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 */

#ifndef TYPES_H
#define TYPES_H

namespace Types
{

    // Defines the possible corners of the screen and their corresponding buttons
    enum Corner
    {
        TL, // Top Left
        TR, // Top Right
        BL, // Bottom Left
        BR, // Bottom Right
    };
}

#endif // TYPES_H