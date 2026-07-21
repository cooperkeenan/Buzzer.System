#ifndef I_UNLOCK_REQUEST_SOURCE_H
#define I_UNLOCK_REQUEST_SOURCE_H

#include <Arduino.h>

/**
 * @brief A door unlock request raised in the cloud
 */
struct UnlockRequest {
    String id;
    unsigned long requestedAt;  // Epoch seconds, set by the cloud webhook
};

/**
 * @brief Interface for retrieving pending unlock requests
 *
 * The device polls this instead of accepting inbound connections,
 * so no port forwarding or public IP is required.
 */
class IUnlockRequestSource {
public:
    virtual ~IUnlockRequestSource() = default;

    /**
     * @brief Fetch the outstanding unlock request, if any
     * @param out Populated only when a pending request exists
     * @return true if a pending request was found, false otherwise
     */
    virtual bool fetchPending(UnlockRequest& out) = 0;

    /**
     * @brief Mark a request as consumed so it is not returned again
     * @param id The request that has been dealt with
     * @return true if the acknowledgement was written, false otherwise
     */
    virtual bool acknowledge(const String& id) = 0;
};

#endif
