#include "UnlockPolicy.h"

UnlockPolicy::UnlockPolicy(ITimeProvider* timeProvider, unsigned long freshnessWindowSeconds)
    : timeProvider(timeProvider), freshnessWindowSeconds(freshnessWindowSeconds) {
}

bool UnlockPolicy::shouldHonor(const UnlockRequest& request) const {
    if (request.id == lastHandledId) {
        Serial.printf("⚠ Unlock request %s already handled - ignoring\n", request.id.c_str());
        return false;
    }

    // Fail closed: without trustworthy time, freshness cannot be judged and
    // a stale request could open the door unattended.
    if (!timeProvider->isSynchronized()) {
        Serial.println("✗ Clock not synchronized - refusing unlock request");
        return false;
    }

    unsigned long now = timeProvider->epochSeconds();

    // A request timestamped ahead of us is clock skew between the cloud and the
    // device, not staleness. Treat it as brand new rather than letting the
    // unsigned subtraction below wrap into a huge age.
    unsigned long age = (request.requestedAt > now) ? 0 : (now - request.requestedAt);

    if (age > freshnessWindowSeconds) {
        Serial.printf("⚠ Unlock request %s is stale (%lus old) - ignoring\n",
                      request.id.c_str(), age);
        return false;
    }

    return true;
}

void UnlockPolicy::markHandled(const String& id) {
    lastHandledId = id;
}
