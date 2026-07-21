#ifndef I_WATCHDOG_H
#define I_WATCHDOG_H

/**
 * @brief Interface for watchdog timer management
 * 
 * Provides abstraction for watchdog timer operations.
 * Follows Interface Segregation Principle - minimal, focused interface.
 */
class IWatchdog {
public:
    virtual ~IWatchdog() = default;
    
    /**
     * @brief Initialize the watchdog timer
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Reset/feed the watchdog timer
     */
    virtual void reset() = 0;
};

#endif