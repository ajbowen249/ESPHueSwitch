#pragma once

#include "IWiFiController.h"

namespace EHS {
class ISupportObjectBundle {
  public:
    // IMPROVE: These should really be shared pointers. Having an issue with them on the ESP.
    IWiFiController* getWiFiController();
};
} // namespace EHS
