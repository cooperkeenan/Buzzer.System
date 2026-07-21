#ifndef NTP_TIME_PROVIDER_H
#define NTP_TIME_PROVIDER_H

#include <Arduino.h>
#include "interfaces/ITimeProvider.h"

/**
 * @brief Wall-clock time sourced from an NTP server
 *
 * UTC only - the device compares timestamps, it never displays them,
 * so no timezone handling is needed.
 */
class NtpTimeProvider : public ITimeProvider {
public:
    NtpTimeProvider(const char* ntpServer, int syncTimeoutMs);

    bool synchronize() override;
    unsigned long epochSeconds() const override;
    bool isSynchronized() const override;

private:
    const char* ntpServer;
    int syncTimeoutMs;
    bool synchronized;

    // Any epoch below this means the clock still holds its power-on default
    // rather than a real network time.
    static constexpr unsigned long MIN_VALID_EPOCH = 1700000000UL;
};

#endif
