#include "FirebaseRestClient.h"
#include "Config.h"

FirebaseRestClient::FirebaseRestClient(
    const char* host,
    const char* authToken,
    IWiFiManager* wifiManager,
    IWatchdog* watchdog
) : host(host), authToken(authToken), wifiManager(wifiManager), watchdog(watchdog) {
    secureClient.setInsecure();

    // Keep the connection alive across polls so we pay for the TLS handshake once.
    http.setReuse(true);
    http.setTimeout(Config::Firebase::REQUEST_TIMEOUT_MS);
}

bool FirebaseRestClient::get(const char* path, String& outPayload) {
    return send("GET", path, String(), &outPayload);
}

bool FirebaseRestClient::patch(const char* path, const String& json) {
    return send("PATCH", path, json, nullptr);
}

String FirebaseRestClient::buildUrl(const char* path) const {
    String url = String("https://") + host + path;

    if (authToken != nullptr && strlen(authToken) > 0) {
        url += "?auth=";
        url += authToken;
    }

    return url;
}

bool FirebaseRestClient::send(
    const char* method,
    const char* path,
    const String& body,
    String* outPayload
) {
    if (!wifiManager->isConnected()) {
        Serial.println("✗ WiFi not connected - skipping Firebase request");
        return false;
    }

    if (!http.begin(secureClient, buildUrl(path))) {
        Serial.println("✗ Failed to begin Firebase request");
        return false;
    }

    watchdog->reset();

    int httpCode;
    if (body.length() > 0) {
        http.addHeader("Content-Type", "application/json");
        httpCode = http.sendRequest(method, (uint8_t*)body.c_str(), body.length());
    } else {
        httpCode = http.sendRequest(method);
    }

    watchdog->reset();

    bool success = (httpCode == HTTP_CODE_OK);

    if (success) {
        if (outPayload != nullptr) {
            *outPayload = http.getString();
        }
    } else {
        Serial.printf("✗ Firebase %s %s failed (HTTP %d)\n", method, path, httpCode);
    }

    http.end();
    return success;
}
