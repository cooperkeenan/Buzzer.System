#include "WatchdogManager.h"
#include <esp_task_wdt.h>
#include <Arduino.h>

WatchdogManager::WatchdogManager(int timeoutSeconds) 
    : timeoutSeconds(timeoutSeconds) {
}

bool WatchdogManager::initialize() {
    Serial.println("Initializing watchdog timer...");
    
    esp_err_t result = esp_task_wdt_init(timeoutSeconds, true);
    
    if (result == ESP_OK) {
        esp_task_wdt_add(NULL);
        Serial.printf("✓ Watchdog enabled (%d second timeout)\n", timeoutSeconds);
        return true;
    } else {
        Serial.printf("✗ Watchdog initialization failed: %d\n", result);
        return false;
    }
}

void WatchdogManager::reset() {
    esp_task_wdt_reset();
}