#ifndef UNLOCK_REQUEST_SOURCE_H
#define UNLOCK_REQUEST_SOURCE_H

#include "interfaces/IUnlockRequestSource.h"
#include "interfaces/IFirebaseRestClient.h"

/**
 * @brief Reads pending unlock requests from Firebase
 *
 * Data access only - whether a request should actually open the door
 * is decided by UnlockPolicy.
 * Depends on IFirebaseRestClient abstraction (Dependency Inversion Principle).
 */
class UnlockRequestSource : public IUnlockRequestSource {
public:
    UnlockRequestSource(IFirebaseRestClient* restClient, const char* requestPath);

    bool fetchPending(UnlockRequest& out) override;
    bool acknowledge(const String& id) override;

private:
    IFirebaseRestClient* restClient;
    const char* requestPath;
};

#endif
