#ifndef INTERVAL_TIMER_H
#define INTERVAL_TIMER_H

#include <Arduino.h>

/**
 * @brief Non-blocking periodic trigger
 *
 * Keeps scheduling out of the code being scheduled. Deliberately avoids
 * delay() so the watchdog and web server keep running between polls.
 * Unsigned subtraction makes this correct across millis() rollover.
 */
class IntervalTimer {
public:
    explicit IntervalTimer(unsigned long intervalMs)
        : intervalMs(intervalMs), lastTriggeredMs(0) {
    }

    /**
     * @brief Whether the interval has passed, rearming the timer if so
     * @param nowMs Current millis() value
     * @return true if the caller should act now, false otherwise
     */
    bool elapsed(unsigned long nowMs) {
        if (nowMs - lastTriggeredMs < intervalMs) {
            return false;
        }

        lastTriggeredMs = nowMs;
        return true;
    }

private:
    unsigned long intervalMs;
    unsigned long lastTriggeredMs;
};

#endif
