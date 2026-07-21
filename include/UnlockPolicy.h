#ifndef UNLOCK_POLICY_H
#define UNLOCK_POLICY_H

#include <Arduino.h>
#include "interfaces/IUnlockRequestSource.h"
#include "interfaces/ITimeProvider.h"

/**
 * @brief Decides whether a pending unlock request may open the door
 *
 * Holds the safety rules that guard the relay, kept separate from both
 * the transport and the orchestration so they can be reasoned about
 * and tested on their own (Single Responsibility Principle).
 *
 * Two independent guards:
 *  - Freshness: a request older than the window is refused, so a reboot
 *    after an outage cannot replay a stale request onto an empty doorstep.
 *  - Replay: an id already acted on is refused, covering the case where
 *    acknowledging the request back to Firebase failed.
 */
class UnlockPolicy {
public:
    UnlockPolicy(ITimeProvider* timeProvider, unsigned long freshnessWindowSeconds);

    /**
     * @brief Whether this request should activate the relay
     * @param request The pending request under consideration
     * @return true if the door may open, false otherwise
     */
    bool shouldHonor(const UnlockRequest& request) const;

    /**
     * @brief Record that a request has been acted on
     */
    void markHandled(const String& id);

private:
    ITimeProvider* timeProvider;
    unsigned long freshnessWindowSeconds;
    String lastHandledId;
};

#endif
