#ifndef FIREBASE_REST_CLIENT_H
#define FIREBASE_REST_CLIENT_H

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "interfaces/IFirebaseRestClient.h"
#include "interfaces/IWiFiManager.h"
#include "interfaces/IWatchdog.h"

/**
 * @brief Firebase Realtime Database REST transport over HTTPS
 *
 * The single owner of TLS and HTTP concerns for the whole application.
 * The client and connection are held open between calls because a fresh
 * TLS handshake costs 1-2 seconds on an ESP32, which would otherwise
 * dominate the polling interval.
 */
class FirebaseRestClient : public IFirebaseRestClient {
public:
    FirebaseRestClient(
        const char* host,
        const char* authToken,
        IWiFiManager* wifiManager,
        IWatchdog* watchdog
    );

    bool get(const char* path, String& outPayload) override;
    bool patch(const char* path, const String& json) override;

private:
    const char* host;
    const char* authToken;
    IWiFiManager* wifiManager;
    IWatchdog* watchdog;

    WiFiClientSecure secureClient;
    HTTPClient http;

    String buildUrl(const char* path) const;
    bool send(const char* method, const char* path, const String& body, String* outPayload);
};

#endif
