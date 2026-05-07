/**
 * Modified version of the Arduino Helpers Timer class. This timer is non-blocking unlike
 * the built in Arduino delay function so it can allow for complex timing logic while still
 * detecting other actions like button presses.
 *
 * Author: Philip Brown (https://github.com/pwbrown)
 * Original Author: Pieter Pas (https://github.com/tttapa)
 * Original Source: https://github.com/tttapa/Arduino-Helpers/blob/master/src/AH/Timing/MillisMicrosTimer.hpp
 */

#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

// Definition for a time function (like millis or micros)
using timefunction = unsigned long (*)();

// Templated timer class that can use either millis or micros
template <timefunction time = micros>
class Timer
{
public:
    /** Starts the timer at the given interval */
    void start(unsigned long newInterval = 0)
    {
        interval = newInterval;
        previous = time();
    }

    /** Checks if the timer has reached the interval and updates the previous timestamp for the next check */
    bool check()
    {
        auto now = time();
        if (now - previous >= interval)
        {
            previous = now;
            return true;
        }
        return false;
    }

private:
    unsigned long interval = 0; // Holds the interval duration (units based on the time function used)
    unsigned long previous = 0; // Holds the timestamp of the last timer start or successful check (units based on the time function used)
};

#endif // TIMER_H