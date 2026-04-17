// Heavily modified version based on the original Arduino Helpers timer class
// Author: Pieter Pas (https://github.com/tttapa)
// Original source: https://github.com/tttapa/Arduino-Helpers/blob/master/src/AH/Timing/MillisMicrosTimer.hpp

#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

using timefunction = unsigned long (*)();

template <timefunction time = micros>
class Timer {
    public:
        /** Starts the timer at the given interval */
        void start(unsigned long newInterval = 0) {
            interval = newInterval;
            previous = time();
        }

        /**
         * boolean operator overload to automatically check the timer for an interval
         * trigger and progress the previous value for the next check
         */
        explicit operator bool() {
            auto now = time();
            if (now - previous >= interval) {
                previous += interval;
                return true;
            }
            return false;
        }

    private:
        unsigned long interval = 0;
        unsigned long previous = 0;
};


#endif // TIMER_H