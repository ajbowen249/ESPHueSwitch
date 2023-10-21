#pragma once

#include "IWiFiController.h"

#include <string>

namespace EHS {
class WiFiControllerImpl : public IWiFiController {
  public:
    WiFiControllerImpl();

    virtual bool ConnectWiFi(wl_status_t& result, unsigned long timeoutLength) override;
    void StartAccessPoint();

    virtual const WiFiSettings& getWiFiSettings() const override;
    virtual void setWiFiSettings(const WiFiSettings& settings) override;

    virtual const WiFiStatus& getWiFiStatus() const override;

  private:
    WiFiStatus _status;
    WiFiSettings _wifiSettings;

    bool _useAccessPoint;
    bool _connectWiFi;

    void applySettings();
};
} // namespace EHS
