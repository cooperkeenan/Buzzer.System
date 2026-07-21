#include <Arduino.h>
#include "DoorOpenerApp.h"

DoorOpenerApp* app = nullptr;

void setup() {
    app = new DoorOpenerApp();
    
    if (!app->initialize()) {
        Serial.println("\n✗✗✗ APPLICATION INITIALIZATION FAILED ✗✗✗");
        Serial.println("System will restart in 5 seconds...");
        delay(5000);
        ESP.restart();
    }
}

void loop() {
    if (app != nullptr) {
        app->run();
    }
}
