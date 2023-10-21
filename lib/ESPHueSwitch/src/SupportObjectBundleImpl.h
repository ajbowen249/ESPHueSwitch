#pragma once

#include "ISupportObjectBundle.h"
#include "WiFiControllerImpl.h"

namespace EHS {
class SupportObjectBundleImpl : public ISupportObjectBundle {
  public:
    SupportObjectBundleImpl();

    IWiFiController* getWiFiController();

  private:
    WiFiControllerImpl _wifiController;
};
} // namespace EHS
