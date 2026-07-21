#include "FirebaseClient.h"
#include <ArduinoJson.h>

FirebaseClient::FirebaseClient(IFirebaseRestClient* restClient, const char* codePath)
    : restClient(restClient), codePath(codePath), lastOperationSuccess(false) {
}

String FirebaseClient::fetchCurrentCode() {
    lastOperationSuccess = false;

    String payload;
    if (!restClient->get(codePath, payload)) {
        return "";
    }

    String code = parseJsonCode(payload);

    if (code.length() == 0) {
        Serial.println("✗ No access code configured in Firebase");
        return "";
    }

    lastOperationSuccess = true;
    return code;
}

bool FirebaseClient::isLastOperationSuccessful() const {
    return lastOperationSuccess;
}

String FirebaseClient::parseJsonCode(const String& payload) const {
    if (payload.length() == 0 || payload == "null") {
        return "";
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.printf("✗ Access code parse error: %s\n", error.c_str());
        return "";
    }

    String code = doc.as<String>();
    code.trim();

    return code;
}
