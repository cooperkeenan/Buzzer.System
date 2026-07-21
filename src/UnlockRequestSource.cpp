#include "UnlockRequestSource.h"
#include <ArduinoJson.h>

namespace {
    constexpr const char* STATUS_PENDING = "pending";
    constexpr const char* STATUS_CONSUMED = "consumed";
}

UnlockRequestSource::UnlockRequestSource(IFirebaseRestClient* restClient, const char* requestPath)
    : restClient(restClient), requestPath(requestPath) {
}

bool UnlockRequestSource::fetchPending(UnlockRequest& out) {
    String payload;

    if (!restClient->get(requestPath, payload)) {
        return false;
    }

    // An empty slot is the normal resting state, not an error.
    if (payload.length() == 0 || payload == "null") {
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.printf("✗ Unlock request parse error: %s\n", error.c_str());
        return false;
    }

    const char* status = doc["status"];
    if (status == nullptr || strcmp(status, STATUS_PENDING) != 0) {
        return false;
    }

    out.id = doc["id"].as<String>();
    out.requestedAt = doc["requestedAt"].as<unsigned long>();

    if (out.id.length() == 0) {
        Serial.println("✗ Pending unlock request has no id - ignoring");
        return false;
    }

    return true;
}

bool UnlockRequestSource::acknowledge(const String& id) {
    String json = String("{\"status\":\"") + STATUS_CONSUMED + "\"}";

    if (!restClient->patch(requestPath, json)) {
        Serial.printf("✗ Failed to acknowledge unlock request %s\n", id.c_str());
        return false;
    }

    return true;
}
