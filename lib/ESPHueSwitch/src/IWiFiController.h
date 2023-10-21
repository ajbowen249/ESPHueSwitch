#pragma once

#include "wl_definitions.h"

#include <string>

namespace EHS {
struct WiFiSettings {
    std::string ssid;
    std::string password;
};

struct WiFiStatus {
    bool isConnected;
    wl_status_t lastResult;
};

class IWiFiController {
  public:
    virtual bool ConnectWiFi(wl_status_t& result, unsigned long timeoutLength);
    virtual void StartAccessPoint();

    virtual const WiFiSettings& getWiFiSettings() const;
    virtual void setWiFiSettings(const WiFiSettings& settings);

    virtual const WiFiStatus& getWiFiStatus() const;
};
} // namespace EHS
