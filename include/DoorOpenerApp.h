#ifndef DOOR_OPENER_APP_H
#define DOOR_OPENER_APP_H

#include <memory>

#include "interfaces/IWatchdog.h"
#include "interfaces/IWiFiManager.h"
#include "interfaces/IFirebaseClient.h"
#include "interfaces/IFirebaseRestClient.h"
#include "interfaces/IRelayController.h"
#include "interfaces/IUnlockRequestSource.h"
#include "interfaces/ITimeProvider.h"
#include "WebServerController.h"
#include "UnlockPolicy.h"
#include "IntervalTimer.h"

/**
 * @brief Main application orchestrator
 *
 * Wires together all components and manages application lifecycle.
 * Follows Dependency Inversion Principle - holds abstractions and injects
 * concrete implementations at construction. This is the composition root.
 */
class DoorOpenerApp {
public:
    DoorOpenerApp();

    /**
     * @brief Initialize all components
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Main application loop
     */
    void run();

private:
    // Component instances (ownership)
    std::unique_ptr<IWatchdog> watchdog;
    std::unique_ptr<IWiFiManager> wifiManager;
    std::unique_ptr<ITimeProvider> timeProvider;
    std::unique_ptr<IFirebaseRestClient> restClient;
    std::unique_ptr<IFirebaseClient> firebaseClient;
    std::unique_ptr<IUnlockRequestSource> unlockRequests;
    std::unique_ptr<IRelayController> relayController;
    std::unique_ptr<UnlockPolicy> unlockPolicy;
    std::unique_ptr<WebServerController> webServer;

    IntervalTimer pollTimer;

    /**
     * @brief Collect and act on any unlock request raised in the cloud
     */
    void processUnlockRequests();

    void printStartupBanner() const;
    void printReadyMessage() const;
};

#endif
