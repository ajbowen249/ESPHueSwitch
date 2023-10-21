#include "ConfigurationServer.h"

#define PORT 80

const char* index_html = R"""(
<html>
    <head>
        <title>
            Hue Switch Config
        </title>
    </head>
    <body>
        <h1>Hue Switch Config</h1>

        <h2>WiFi</h2>
        SSID: %s<br />
        Status: %s<br /><br />
        Set WiFi Credentials:<br />
        <label for="ssid">SSID: </label>
        <input id="ssid" type="text" /><br />

        <label for="password">Password: </label>
        <input id="password" type="password" /><br />

        <button>Submit</button>
    </body>
</html>
)""";

#define RESPONSE_BUFFER_SIZE 2048
char response_buffer[RESPONSE_BUFFER_SIZE];

void onUnhandledRequest(AsyncWebServerRequest* request) { request->send(404); }

EHS::ConfigurationServer::ConfigurationServer(EHS::ISupportObjectBundle* supportObjects)
    : _supportObjects(supportObjects), _server(PORT) {}

void EHS::ConfigurationServer::start() {
    _server.reset();

    const auto wifiController = _supportObjects->getWiFiController();

    _server.on("/", HTTP_GET, [=](AsyncWebServerRequest* request) {
        const auto wifiStatus = wifiController->getWiFiStatus();
        const auto wifiSettings = wifiController->getWiFiSettings();

        snprintf(
            response_buffer, RESPONSE_BUFFER_SIZE, index_html,
            wifiSettings.ssid.c_str(),
            wifiStatus.isConnected ? "Connected" : "Not Connected"
        );

        AsyncWebServerResponse* response = request->beginResponse(
            200,
            "text/html",
            response_buffer
        );
        request->send(response);
    });

    _server.onNotFound(onUnhandledRequest);

    _server.begin();
}

void EHS::ConfigurationServer::stop() { _server.end(); }
