#include "RelayController.h"
#include <Arduino.h>

RelayController::RelayController(int pin, int durationMs, bool activeHigh, IWatchdog* watchdog)
    : pin(pin), durationMs(durationMs), activeHigh(activeHigh), watchdog(watchdog), active(false) {
}

bool RelayController::initialize() {
    pinMode(pin, OUTPUT);
    setRelayState(false);
    
    Serial.printf("✓ Relay initialized on GPIO %d (Active %s)\n", 
                  pin, activeHigh ? "HIGH" : "LOW");
    
    return true;
}

void RelayController::activate() {
    if (active) {
        Serial.println("⚠ Relay already active, ignoring request");
        return;
    }
    
    Serial.println("\n=== ACTIVATING RELAY ===");
    active = true;
    
    setRelayState(true);
    Serial.println("Relay ON - Door unlocking...");
    
    unsigned long startTime = millis();
    while (millis() - startTime < durationMs) {
        watchdog->reset();
        delay(100);
    }
    
    setRelayState(false);
    Serial.println("Relay OFF - Door lock restored");
    
    active = false;
}

bool RelayController::isActive() const {
    return active;
}

void RelayController::setRelayState(bool state) {
    bool pinState = activeHigh ? state : !state;
    digitalWrite(pin, pinState ? HIGH : LOW);
}