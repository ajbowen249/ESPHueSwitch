#pragma once

#include "IWiFiController.h"
#include "IHueController.h"

namespace EHS {
class ISupportObjectBundle {
  public:
    // IMPROVE: These should really be shared pointers. Having an issue with them on the ESP.
    virtual IWiFiController* getWiFiController();
    virtual IHueController* getHueController();
};
} // namespace EHS
