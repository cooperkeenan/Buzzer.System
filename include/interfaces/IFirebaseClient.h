#ifndef I_FIREBASE_CLIENT_H
#define I_FIREBASE_CLIENT_H

#include <Arduino.h>

/**
 * @brief Interface for Firebase database operations
 * 
 * Provides abstraction for Firebase REST API calls.
 * Allows for testing with mock implementations.
 */
class IFirebaseClient {
public:
    virtual ~IFirebaseClient() = default;
    
    /**
     * @brief Fetch the current valid access code from Firebase
     * @return The access code, or empty string on failure
     */
    virtual String fetchCurrentCode() = 0;
    
    /**
     * @brief Check if the last operation was successful
     * @return true if last operation succeeded, false otherwise
     */
    virtual bool isLastOperationSuccessful() const = 0;
};

#endif