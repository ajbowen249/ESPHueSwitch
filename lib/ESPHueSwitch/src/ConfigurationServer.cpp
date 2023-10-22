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

        <h2>Status</h2>
        <h3>WiFi</h3>
        Status: %s<br />
        SSID: %s<br />

        <h2>Setup</h2>

        <ul>
            <li><a href="/update_wifi.html">WiFi Setup</a></li>
        </ul>
    </body>
</html>
)""";


const char* update_wifi_html = R"""(
<html>
    <head>
        <title>
            Hue Switch Config
        </title>
    </head>
    <body>
        <h1>WiFi Setup</h1>
        Status: %s<br />
        SSID: %s<br /><br />

        Set WiFi Credentials:<br />
        <form action="/update_wifi_result.html" method="get">
            <label for="ssid">SSID: </label>
            <input id="ssid" name="ssid" type="text" /><br />

            <label for="password">Password: </label>
            <input id="password" name="password" type="password" /><br />

            <button type="submit" formmethod="get">Update</button>
        </form>
    </body>
</html>
)""";

const char* update_wifi_result_html = R"""(
<html>
    <head>
        <title>
            Hue Switch Config
        </title>
        <script type="application/javascript">
            function onLoad() {
                setTimeout(() => {
                    window.location.pathname = '/';
                }, 10000);
            }
        </script>
    </head>
    <body onload="onLoad();">
        <h1>Updating WiFi...</h1>
        Applying WiFi settings. You should be redirected momentarily.
    </body>
</html>
)""";

const char* error_html = R"""(
<html>
    <head>
        <title>
            Hue Switch Config
        </title>
    </head>
    <body>
        An unknown error has occurred.
    </body>
</html>
)""";

#define RESPONSE_BUFFER_SIZE 2048
char response_buffer[RESPONSE_BUFFER_SIZE];

#define QUERY_SSID "ssid"
#define QUERY_PASSWORD "password"

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

    _server.on("/update_wifi.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        const auto wifiStatus = wifiController->getWiFiStatus();
        const auto wifiSettings = wifiController->getWiFiSettings();

        snprintf(
            response_buffer, RESPONSE_BUFFER_SIZE, update_wifi_html,
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

    _server.on("/update_wifi_result.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        if (!request->hasParam(QUERY_SSID) || !request->hasParam(QUERY_PASSWORD)) {
            Serial.println("rejecting for params");
            AsyncWebServerResponse* response = request->beginResponse(
                200,
                "text/html",
                error_html
            );

            request->send(response);
            return;
        }

        Serial.println("responding...");
        AsyncWebServerResponse* response = request->beginResponse(
            200,
            "text/html",
            update_wifi_result_html
        );

        request->send(response);

        Serial.println("about to get param values");
        const auto ssidParam = request->getParam(QUERY_SSID);
        const auto passwordParam = request->getParam(QUERY_PASSWORD);

        Serial.println("got values");
        Serial.println(ssidParam->value().c_str());
        Serial.println(passwordParam->value().c_str());

        Serial.println("about to set values...");
        wifiController->setWiFiSettings({
            ssidParam->value().c_str(),
            passwordParam->value().c_str(),
        });

        Serial.println("flagging wifi reconnect...");
        wifiController->flagReconnect();
    });

    _server.onNotFound(onUnhandledRequest);

    _server.begin();
}

void EHS::ConfigurationServer::stop() { _server.end(); }
