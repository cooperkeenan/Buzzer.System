#include "WiFiManager.h"
#include <WiFi.h>
#include "Config.h"

WiFiManager::WiFiManager(const char* ssid, const char* password, IWatchdog* watchdog)
    : ssid(ssid), password(password), watchdog(watchdog) {
}

bool WiFiManager::connect() {
    Serial.println("\n=== WiFi Connection ===");
    Serial.printf("Connecting to: %s\n", ssid);

    // DHCP rather than a fixed address: all traffic is outbound now, so nothing
    // depends on this device having a predictable IP. Keeps the firmware portable
    // across routers whose subnets differ.
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    unsigned long startTime = millis();
    
    while (!isConnected() && (millis() - startTime < Config::WiFi::CONNECTION_TIMEOUT_MS)) {
        delay(500);
        Serial.print(".");
        watchdog->reset();
    }
    
    Serial.println();
    
    if (isConnected()) {
        printConnectionInfo();
        return true;
    } else {
        Serial.println("✗ WiFi connection failed");
        return false;
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIPAddress() const {
    return WiFi.localIP().toString();
}

int WiFiManager::getSignalStrength() const {
    return WiFi.RSSI();
}

void WiFiManager::ensureConnection() {
    if (!isConnected()) {
        Serial.println("WiFi disconnected! Reconnecting...");
        connect();
    }
}

void WiFiManager::printConnectionInfo() const {
    Serial.println("✓ WiFi connected!");
    Serial.printf("  IP Address: %s\n", getIPAddress().c_str());
    Serial.printf("  Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("  Subnet: %s\n", WiFi.subnetMask().toString().c_str());
    Serial.printf("  DNS: %s\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("  Signal: %d dBm\n", getSignalStrength());
}