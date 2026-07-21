#ifndef WEB_SERVER_CONTROLLER_H
#define WEB_SERVER_CONTROLLER_H

#include <WebServer.h>
#include "interfaces/IFirebaseClient.h"
#include "interfaces/IRelayController.h"
#include "interfaces/IWiFiManager.h"
#include "interfaces/IWatchdog.h"

/**
 * @brief Web server controller for the LAN-only HTTP API
 *
 * Provides a status page and a manual unlock endpoint for use on the local
 * network. SMS unlocking no longer arrives here - it is handled in the cloud
 * and collected by polling, so this server needs no inbound port forward.
 * Follows Single Responsibility Principle - only manages web server and request routing.
 * Uses Dependency Inversion - depends on abstractions, not concrete implementations.
 */
class WebServerController {
public:
    WebServerController(
        int port,
        IFirebaseClient* firebaseClient,
        IRelayController* relayController,
        IWiFiManager* wifiManager,
        IWatchdog* watchdog
    );
    
    /**
     * @brief Start the web server
     */
    void begin();
    
    /**
     * @brief Process incoming HTTP requests (call in loop)
     */
    void handleClient();

private:
    WebServer server;
    IFirebaseClient* firebaseClient;
    IRelayController* relayController;
    IWiFiManager* wifiManager;
    IWatchdog* watchdog;
    
    // HTTP Request Handlers
    void handleRoot();
    void handleUnlock();
    void handleNotFound();

    // Helper Methods
    bool validateCode(const String& submittedCode);
    void sendJsonResponse(int statusCode, const String& status, const String& message);
};

#endif