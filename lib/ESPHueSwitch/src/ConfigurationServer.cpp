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
        WiFi Credentials:<br />
        <label for="ssid">SSID: </label>
        <input id="ssid" type="text" /><br />

        <label for="password">Password: </label>
        <input id="password" type="password" /><br />

        <button>Submit</button>
    </body>
</html>
)""";

void onUnhandledRequest(AsyncWebServerRequest* request) { request->send(404); }

EHS::ConfigurationServer::ConfigurationServer(EHS::ISupportObjectBundle* supportObjects)
    : _supportObjects(supportObjects), _server(PORT) {}

void EHS::ConfigurationServer::start() {
    _server.reset();

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", index_html);
        request->send(response);
    });

    _server.onNotFound(onUnhandledRequest);

    _server.begin();
}

void EHS::ConfigurationServer::stop() { _server.end(); }
