#pragma once

#include "ISupportObjectBundle.h"

#include "ESPAsyncWebServer.h"

namespace EHS {
class ConfigurationServer {
  public:
    ConfigurationServer(ISupportObjectBundle* supportObjects);

    void start();
    void stop();

  private:
    ISupportObjectBundle* _supportObjects;

    AsyncWebServer _server;
};
} // namespace EHS
