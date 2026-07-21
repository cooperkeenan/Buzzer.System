#ifndef FIREBASE_CLIENT_H
#define FIREBASE_CLIENT_H

#include "interfaces/IFirebaseClient.h"
#include "interfaces/IFirebaseRestClient.h"

/**
 * @brief Reads the current access code from Firebase
 *
 * Used by the LAN-only /unlock endpoint. The SMS path validates codes in the
 * cloud instead, so this is no longer on the critical path for door access.
 * Depends on IFirebaseRestClient abstraction (Dependency Inversion Principle).
 */
class FirebaseClient : public IFirebaseClient {
public:
    FirebaseClient(IFirebaseRestClient* restClient, const char* codePath);

    String fetchCurrentCode() override;
    bool isLastOperationSuccessful() const override;

private:
    IFirebaseRestClient* restClient;
    const char* codePath;
    bool lastOperationSuccess;

    String parseJsonCode(const String& payload) const;
};

#endif
