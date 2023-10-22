#pragma once

#include "ISupportObjectBundle.h"
#include "WiFiControllerImpl.h"
#include "HueControllerImpl.h"

namespace EHS {
class SupportObjectBundleImpl : public ISupportObjectBundle {
  public:
    SupportObjectBundleImpl();

    virtual IWiFiController* getWiFiController() override;
    virtual IHueController* getHueController() override;

  private:
    WiFiControllerImpl _wifiController;
    HueControllerImpl _hueController;
};
} // namespace EHS
