#ifndef I_RELAY_CONTROLLER_H
#define I_RELAY_CONTROLLER_H

/**
 * @brief Interface for relay control operations
 * 
 * Provides abstraction for hardware relay control.
 * Enables easy testing with mock implementations.
 */
class IRelayController {
public:
    virtual ~IRelayController() = default;
    
    /**
     * @brief Initialize the relay hardware
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Activate the relay for a specified duration
     * This method blocks until the relay is deactivated
     */
    virtual void activate() = 0;
    
    /**
     * @brief Check if relay is currently active
     * @return true if relay is active, false otherwise
     */
    virtual bool isActive() const = 0;
};

#endif