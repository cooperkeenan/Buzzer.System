#include "NtpTimeProvider.h"
#include <time.h>

NtpTimeProvider::NtpTimeProvider(const char* ntpServer, int syncTimeoutMs)
    : ntpServer(ntpServer), syncTimeoutMs(syncTimeoutMs), synchronized(false) {
}

bool NtpTimeProvider::synchronize() {
    Serial.printf("Syncing time from %s...\n", ntpServer);

    configTime(0, 0, ntpServer);

    unsigned long startTime = millis();
    while (time(nullptr) < MIN_VALID_EPOCH && (millis() - startTime < (unsigned long)syncTimeoutMs)) {
        delay(200);
    }

    synchronized = (time(nullptr) >= MIN_VALID_EPOCH);

    if (synchronized) {
        Serial.printf("✓ Time synced (epoch %lu)\n", epochSeconds());
    } else {
        Serial.println("✗ Time sync failed");
    }

    return synchronized;
}

unsigned long NtpTimeProvider::epochSeconds() const {
    return synchronized ? (unsigned long)time(nullptr) : 0;
}

bool NtpTimeProvider::isSynchronized() const {
    return synchronized;
}
