#ifndef CONFIG_H
#define CONFIG_H

#include "Secrets.h"

namespace Config {
    // WiFi Configuration
    namespace WiFi {
        // Credentials live in the gitignored Secrets.h - moving house means
        // editing those two values and nothing else.
        constexpr const char* SSID = Secrets::WIFI_SSID;
        constexpr const char* PASSWORD = Secrets::WIFI_PASSWORD;
        constexpr int CONNECTION_TIMEOUT_MS = 30000;
        constexpr int RECONNECT_DELAY_MS = 5000;
    }

    // Firebase Configuration
    namespace Firebase {
    constexpr const char* HOST = "buzzer-system-9d674-default-rtdb.firebaseio.com";
    constexpr const char* CODE_PATH = "/doorAccess/currentCode.json";
    constexpr const char* UNLOCK_REQUEST_PATH = "/doorAccess/unlockRequest.json";
    constexpr int REQUEST_TIMEOUT_MS = 5000;

    // Database secret appended as ?auth=. Required for writes - without it the
    // device cannot acknowledge unlock requests. Lives in the gitignored Secrets.h.
    constexpr const char* AUTH_TOKEN = Secrets::FIREBASE_AUTH_TOKEN;

    // How often the device asks Firebase whether a door unlock is pending.
    constexpr int POLL_INTERVAL_MS = 3000;

    // Unlock requests older than this are ignored, so a reboot after an outage
    // cannot replay a stale request and open the door unattended.
    constexpr unsigned long REQUEST_FRESHNESS_WINDOW_S = 60;
    }

    // Time Configuration
    namespace Time {
        // Unlock requests are timestamped in the cloud, so the device needs real
        // wall-clock time to judge whether one is still fresh.
        constexpr const char* NTP_SERVER = "pool.ntp.org";
        constexpr int SYNC_TIMEOUT_MS = 10000;
    }

    // Hardware Configuration
    namespace Hardware {
        constexpr int RELAY_PIN = 2;
        constexpr int RELAY_DURATION_MS = 3000;
        constexpr bool RELAY_ACTIVE_HIGH = true;
    }

    // Watchdog Configuration
    namespace Watchdog {
        constexpr int TIMEOUT_SECONDS = 30;
    }

    // Web Server Configuration
    namespace WebServer {
        constexpr int PORT = 80;
    }

    // Serial Configuration
    namespace Serial {
        constexpr unsigned long BAUD_RATE = 115200;
    }
}

#endif