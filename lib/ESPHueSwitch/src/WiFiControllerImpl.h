#pragma once

#include "IWiFiController.h"

#include <string>

namespace EHS {
class WiFiControllerImpl : public IWiFiController {
  public:
    WiFiControllerImpl();

    virtual bool connectWiFi(wl_status_t& result, unsigned long timeoutLength) override;
    void startAccessPoint();

    virtual const WiFiSettings& getWiFiSettings() const override;
    virtual void setWiFiSettings(const WiFiSettings& settings) override;
    virtual void flagReconnect() override;

    virtual const WiFiStatus& getWiFiStatus() const override;

    virtual void loop() override;
  private:
    WiFiStatus _status;
    WiFiSettings _wifiSettings;

    bool _useAccessPoint;
    bool _connectWiFi;
    bool _shouldReconnect;

    void applySettings();
};
} // namespace EHS
