#ifndef I_WIFI_MANAGER_H
#define I_WIFI_MANAGER_H

#include <Arduino.h>

/**
 * @brief Interface for WiFi connection management
 * 
 * Provides abstraction for WiFi operations.
 * Allows for easy testing and alternative implementations.
 */
class IWiFiManager {
public:
    virtual ~IWiFiManager() = default;
    
    /**
     * @brief Connect to WiFi network
     * @return true if connected successfully, false otherwise
     */
    virtual bool connect() = 0;
    
    /**
     * @brief Check if currently connected to WiFi
     * @return true if connected, false otherwise
     */
    virtual bool isConnected() const = 0;
    
    /**
     * @brief Get the local IP address
     * @return IP address as String
     */
    virtual String getIPAddress() const = 0;
    
    /**
     * @brief Get WiFi signal strength
     * @return Signal strength in dBm
     */
    virtual int getSignalStrength() const = 0;
    
    /**
     * @brief Attempt to reconnect if disconnected
     */
    virtual void ensureConnection() = 0;
};

#endif