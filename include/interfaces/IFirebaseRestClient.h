#ifndef I_FIREBASE_REST_CLIENT_H
#define I_FIREBASE_REST_CLIENT_H

#include <Arduino.h>

/**
 * @brief Interface for Firebase Realtime Database REST transport
 *
 * Transport concern only - knows about HTTP, TLS and authentication,
 * but nothing about what the data means.
 * Follows Single Responsibility Principle - callers own their own parsing.
 */
class IFirebaseRestClient {
public:
    virtual ~IFirebaseRestClient() = default;

    /**
     * @brief Read a database path
     * @param path Database path including the .json suffix
     * @param outPayload Receives the raw response body on success
     * @return true if the request succeeded, false otherwise
     */
    virtual bool get(const char* path, String& outPayload) = 0;

    /**
     * @brief Merge fields into a database path
     * @param path Database path including the .json suffix
     * @param json Object containing only the fields to update
     * @return true if the request succeeded, false otherwise
     */
    virtual bool patch(const char* path, const String& json) = 0;
};

#endif
