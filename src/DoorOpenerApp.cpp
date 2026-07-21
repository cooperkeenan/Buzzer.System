#include "DoorOpenerApp.h"
#include "Config.h"
#include "WatchdogManager.h"
#include "WiFiManager.h"
#include "NtpTimeProvider.h"
#include "FirebaseRestClient.h"
#include "FirebaseClient.h"
#include "UnlockRequestSource.h"
#include "RelayController.h"
#include <Arduino.h>

DoorOpenerApp::DoorOpenerApp()
    : pollTimer(Config::Firebase::POLL_INTERVAL_MS) {
}

bool DoorOpenerApp::initialize() {
    Serial.begin(Config::Serial::BAUD_RATE);
    delay(1000);

    printStartupBanner();

    // Create watchdog (no dependencies)
    watchdog.reset(new WatchdogManager(Config::Watchdog::TIMEOUT_SECONDS));
    if (!watchdog->initialize()) {
        Serial.println("✗ Failed to initialize watchdog");
        return false;
    }

    // Create WiFi manager (depends on watchdog)
    wifiManager.reset(new WiFiManager(
        Config::WiFi::SSID,
        Config::WiFi::PASSWORD,
        watchdog.get()
    ));
    if (!wifiManager->connect()) {
        Serial.println("✗ Failed to connect to WiFi");
        return false;
    }

    // Create time provider (depends on WiFi being up)
    timeProvider.reset(new NtpTimeProvider(
        Config::Time::NTP_SERVER,
        Config::Time::SYNC_TIMEOUT_MS
    ));
    if (!timeProvider->synchronize()) {
        // Not fatal - run() retries, and UnlockPolicy refuses unlocks until the
        // clock is trustworthy rather than risking a stale request.
        Serial.println("⚠ Continuing without time sync - unlocks disabled until it succeeds");
    }

    // Create shared Firebase transport (depends on WiFi and watchdog)
    restClient.reset(new FirebaseRestClient(
        Config::Firebase::HOST,
        Config::Firebase::AUTH_TOKEN,
        wifiManager.get(),
        watchdog.get()
    ));

    // Create Firebase readers (depend on the transport abstraction)
    firebaseClient.reset(new FirebaseClient(
        restClient.get(),
        Config::Firebase::CODE_PATH
    ));

    unlockRequests.reset(new UnlockRequestSource(
        restClient.get(),
        Config::Firebase::UNLOCK_REQUEST_PATH
    ));

    // Create unlock policy (depends on time provider)
    unlockPolicy.reset(new UnlockPolicy(
        timeProvider.get(),
        Config::Firebase::REQUEST_FRESHNESS_WINDOW_S
    ));

    // Create relay controller (depends on watchdog)
    relayController.reset(new RelayController(
        Config::Hardware::RELAY_PIN,
        Config::Hardware::RELAY_DURATION_MS,
        Config::Hardware::RELAY_ACTIVE_HIGH,
        watchdog.get()
    ));
    if (!relayController->initialize()) {
        Serial.println("✗ Failed to initialize relay");
        return false;
    }

    // Create web server (depends on everything)
    webServer.reset(new WebServerController(
        Config::WebServer::PORT,
        firebaseClient.get(),
        relayController.get(),
        wifiManager.get(),
        watchdog.get()
    ));
    webServer->begin();

    printReadyMessage();
    return true;
}

void DoorOpenerApp::run() {
    watchdog->reset();
    wifiManager->ensureConnection();
    webServer->handleClient();

    if (pollTimer.elapsed(millis())) {
        processUnlockRequests();
    }

    delay(10);
}

void DoorOpenerApp::processUnlockRequests() {
    // A clock that never synced leaves every request unjudgeable, so keep
    // retrying in the background rather than polling for requests we must refuse.
    if (!timeProvider->isSynchronized() && !timeProvider->synchronize()) {
        return;
    }

    UnlockRequest request;
    if (!unlockRequests->fetchPending(request)) {
        return;
    }

    Serial.printf("\n=== UNLOCK REQUEST %s ===\n", request.id.c_str());

    if (!unlockPolicy->shouldHonor(request)) {
        // Clear it regardless, otherwise a refused request is re-read every poll.
        unlockPolicy->markHandled(request.id);
        unlockRequests->acknowledge(request.id);
        return;
    }

    Serial.println("✓ Request accepted - unlocking door!");

    // Mark before activating: the relay blocks for its full duration, and a reset
    // during that window must not let the same request fire again on reboot.
    unlockPolicy->markHandled(request.id);
    unlockRequests->acknowledge(request.id);

    relayController->activate();
}

void DoorOpenerApp::printStartupBanner() const {
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════╗");
    Serial.println("║   ESP32 DOOR OPENER - STARTING     ║");
    Serial.println("╚════════════════════════════════════╝");
    Serial.println();
}

void DoorOpenerApp::printReadyMessage() const {
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║          SYSTEM READY              ║");
    Serial.println("╚════════════════════════════════════╝");
    Serial.printf("Polling Firebase for unlock requests every %dms\n",
                  Config::Firebase::POLL_INTERVAL_MS);
    Serial.printf("Local status page: http://%s/\n", wifiManager->getIPAddress().c_str());
    Serial.println("════════════════════════════════════\n");
}
