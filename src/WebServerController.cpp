#include "WebServerController.h"
#include "Config.h"
#include <functional>

WebServerController::WebServerController(
    int port,
    IFirebaseClient* firebaseClient,
    IRelayController* relayController,
    IWiFiManager* wifiManager,
    IWatchdog* watchdog
) : server(port),
    firebaseClient(firebaseClient),
    relayController(relayController),
    wifiManager(wifiManager),
    watchdog(watchdog) {
}

void WebServerController::begin() {
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/unlock", HTTP_POST, [this]() { handleUnlock(); });
    server.onNotFound([this]() { handleNotFound(); });
    
    server.begin();
    Serial.println("\n✓ Web server started");
}

void WebServerController::handleClient() {
    watchdog->reset();
    server.handleClient();
}

void WebServerController::handleRoot() {
    watchdog->reset();
    
    // Store values in variables first to avoid namespace resolution issues
    String ssid = Config::WiFi::SSID;
    int signalStrength = wifiManager->getSignalStrength();
    String ipAddress = wifiManager->getIPAddress();
    int uptimeSeconds = millis() / 1000;
    
    String html = "<!DOCTYPE html><html><head><title>ESP32 Door Opener</title></head><body>";
    html += "<h1>ESP32 Door Opener</h1>";
    html += "<p><strong>Status:</strong> Online ✓</p>";
    html += "<p><strong>WiFi:</strong> " + ssid + " (" + String(signalStrength) + " dBm)</p>";
    html += "<p><strong>IP:</strong> " + ipAddress + "</p>";
    html += "<p><strong>Uptime:</strong> " + String(uptimeSeconds) + " seconds</p>";
    html += "<hr><p>POST to <code>/unlock</code> with 'code' parameter (LAN only)</p>";
    html += "<p>SMS unlocking is handled in the cloud and collected by polling.</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
}

void WebServerController::handleUnlock() {
    watchdog->reset();
    
    Serial.println("\n=== UNLOCK REQUEST RECEIVED ===");
    
    if (relayController->isActive()) {
        Serial.println("✗ Door operation already in progress");
        sendJsonResponse(429, "error", "Door operation in progress");
        return;
    }
    
    if (!server.hasArg("code")) {
        Serial.println("✗ Missing 'code' parameter");
        sendJsonResponse(400, "error", "Missing code parameter");
        return;
    }
    
    String submittedCode = server.arg("code");
    submittedCode.trim();
    Serial.printf("Submitted code: '%s'\n", submittedCode.c_str());
    
    if (validateCode(submittedCode)) {
        Serial.println("✓ CODE VALID - Unlocking door!");
        relayController->activate();
        sendJsonResponse(200, "success", "Door unlocked");
    } else {
        Serial.println("✗ CODE INVALID");
        sendJsonResponse(403, "error", "Invalid code");
    }
}

void WebServerController::handleNotFound() {
    watchdog->reset();
    sendJsonResponse(404, "error", "Endpoint not found");
}

bool WebServerController::validateCode(const String& submittedCode) {
    String validCode = firebaseClient->fetchCurrentCode();
    
    if (!firebaseClient->isLastOperationSuccessful()) {
        Serial.println("✗ Failed to fetch valid code from Firebase");
        return false;
    }
    
    if (validCode.length() == 0) {
        Serial.println("✗ Received empty code from Firebase");
        return false;
    }
    
    String submittedUpper = submittedCode;
    String validUpper = validCode;
    submittedUpper.toUpperCase();
    validUpper.toUpperCase();
    
    bool isValid = (submittedUpper == validUpper);
    
    if (!isValid) {
        Serial.printf("Code mismatch - Submitted: '%s' vs Valid: '%s'\n", 
                     submittedUpper.c_str(), validUpper.c_str());
    }
    
    return isValid;
}

void WebServerController::sendJsonResponse(int statusCode, const String& status, const String& message) {
    String json = "{\"status\":\"" + status + "\",\"message\":\"" + message + "\"}";
    server.send(statusCode, "application/json", json);
}