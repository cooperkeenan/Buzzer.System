#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include "interfaces/IRelayController.h"
#include "interfaces/IWatchdog.h"

/**
 * @brief GPIO-based relay controller for ESP32
 * 
 * Controls a relay connected to a GPIO pin.
 * Depends on IWatchdog to prevent system lockup during delays.
 */
class RelayController : public IRelayController {
public:
    RelayController(int pin, int durationMs, bool activeHigh, IWatchdog* watchdog);
    
    bool initialize() override;
    void activate() override;
    bool isActive() const override;

private:
    int pin;
    int durationMs;
    bool activeHigh;
    IWatchdog* watchdog;
    bool active;
    
    void setRelayState(bool state);
};

#endif