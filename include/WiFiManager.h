#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "interfaces/IWiFiManager.h"
#include "interfaces/IWatchdog.h"

/**
 * @brief ESP32 WiFi Manager implementation
 * 
 * Manages WiFi connectivity with automatic reconnection.
 * Depends on IWatchdog abstraction (Dependency Inversion Principle).
 */
class WiFiManager : public IWiFiManager {
public:
    WiFiManager(const char* ssid, const char* password, IWatchdog* watchdog);
    
    bool connect() override;
    bool isConnected() const override;
    String getIPAddress() const override;
    int getSignalStrength() const override;
    void ensureConnection() override;

private:
    const char* ssid;
    const char* password;
    IWatchdog* watchdog;
    
    void printConnectionInfo() const;
};

#endif