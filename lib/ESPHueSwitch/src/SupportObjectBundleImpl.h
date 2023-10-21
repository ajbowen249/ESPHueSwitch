#pragma once

#include "ISupportObjectBundle.h"
#include "WiFiControllerImpl.h"

namespace EHS {
class SupportObjectBundleImpl : public ISupportObjectBundle {
  public:
    SupportObjectBundleImpl();

    virtual IWiFiController* getWiFiController() override;

  private:
    WiFiControllerImpl _wifiController;
};
} // namespace EHS
