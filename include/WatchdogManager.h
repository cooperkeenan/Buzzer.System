#ifndef WATCHDOG_MANAGER_H
#define WATCHDOG_MANAGER_H

#include "interfaces/IWatchdog.h"

/**
 * @brief ESP32 Watchdog Timer implementation
 * 
 * Concrete implementation of IWatchdog for ESP32 hardware.
 * Follows Single Responsibility Principle - only manages watchdog.
 */
class WatchdogManager : public IWatchdog {
public:
    explicit WatchdogManager(int timeoutSeconds);
    
    bool initialize() override;
    void reset() override;

private:
    int timeoutSeconds;
};

#endif