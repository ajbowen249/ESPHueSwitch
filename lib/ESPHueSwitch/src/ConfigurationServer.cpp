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

    <h3>Hue</h3>
    IP: %s<br />
    User ID: %s<br />
    Item: %s<br />

    <h2>Setup</h2>
    <ul>
      <li><a href="/update_wifi.html">WiFi Setup</a></li>
      <li><a href="/hue_setup.html">Hue Setup</a></li>
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

const char* wait_and_redirect_html = R"""(
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
  <h1>Updating...</h1>
  Applying settings. You should be redirected momentarily.
  </body>
</html>
)""";

const char* hue_setup_html = R"""(
<html>
  <head>
    <title>
      Hue Switch Config
    </title>
  </head>
  <body>
    <h1>Hue Setup</h1>
    %s<br />

    IP: %s<br />
    User ID: %s<br />
    Item: %s<br /><br />

    Set IP (can be found in Hue app):<br />
    <form action="/set_hue_ip.html" method="get">
      <label for="ip">IP: </label>
      <input id="ip" name="ip" type="text" /><br />

      <button type="submit" formmethod="get">Set</button>
    </form>

    <form action="/get_user_id.html" method="get">
      Set up User ID
      <ol>
        <li>Set an IP address</li>
        <li>Tap button on Hue Bridge</li>
        <li>Press this button</li>
      </ol>
      <button type="submit" formmethod="get">Set Up User ID</button>
    </form>

    <form action="/set_item.html" method="get">
      Select Item
      <label for="id">ID: </label>
      <input id="id" name="id" type="text" /><br />

      <label for="isLight">Is Single Light: </label>
      <input id="isLight" name="isLight" type="checkbox" value="true" /><br />

      <button type="submit" formmethod="get">Set Item</button>
    </form>
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
#define QUERY_IP "ip"
#define QUERY_ID "id"
#define QUERY_IS_LIGHT "isLight"

void onUnhandledRequest(AsyncWebServerRequest* request) { request->send(404); }

EHS::ConfigurationServer::ConfigurationServer(EHS::ISupportObjectBundle* supportObjects)
    : _supportObjects(supportObjects), _server(PORT) {}

void renderHueSetup(EHS::IHueController* hueController, const char* banner) {
    snprintf(
        response_buffer, RESPONSE_BUFFER_SIZE, hue_setup_html,
        banner,
        hueController->getIP().c_str(),
        hueController->getUserId().c_str(),
        hueController->getItem().id.c_str()
    );
}

void EHS::ConfigurationServer::start() {
    _server.reset();

    const auto wifiController = _supportObjects->getWiFiController();
    const auto hueController = _supportObjects->getHueController();

    _server.on("/", HTTP_GET, [=](AsyncWebServerRequest* request) {
        const auto wifiStatus = wifiController->getWiFiStatus();
        const auto wifiSettings = wifiController->getWiFiSettings();

        snprintf(
            response_buffer, RESPONSE_BUFFER_SIZE, index_html,
            wifiSettings.ssid.c_str(),
            wifiStatus.isConnected ? "Connected" : "Not Connected",
            hueController->getIP().c_str(),
            hueController->getUserId().c_str(),
            hueController->getItem().id.c_str()
        );

        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);

        request->send(response);
    });

    _server.on("/update_wifi.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        const auto wifiStatus = wifiController->getWiFiStatus();
        const auto wifiSettings = wifiController->getWiFiSettings();

        snprintf(response_buffer, RESPONSE_BUFFER_SIZE, update_wifi_html, wifiSettings.ssid.c_str(),
                 wifiStatus.isConnected ? "Connected" : "Not Connected");

        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);

        request->send(response);
    });

    _server.on("/update_wifi_result.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        if (!request->hasParam(QUERY_SSID) || !request->hasParam(QUERY_PASSWORD)) {
            AsyncWebServerResponse* response = request->beginResponse(200, "text/html", error_html);

            request->send(response);
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", wait_and_redirect_html);

        request->send(response);

        const auto ssidParam = request->getParam(QUERY_SSID);
        const auto passwordParam = request->getParam(QUERY_PASSWORD);

        wifiController->setWiFiSettings({
            ssidParam->value().c_str(),
            passwordParam->value().c_str(),
        });

        wifiController->flagReconnect();
    });

    _server.on("/hue_setup.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        renderHueSetup(hueController, "");
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);

        request->send(response);
    });

    _server.on("/set_hue_ip.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        std::string banner = "";

        if (request->hasParam(QUERY_IP)) {
            banner = "IP updated!";
            hueController->setIP(request->getParam(QUERY_IP)->value().c_str());
        } else {
            banner = "Bad request!";
        }

        renderHueSetup(hueController, banner.c_str());

        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);

        request->send(response);
    });

    _server.on("/get_user_id.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        if (hueController->getIP() == "") {
            renderHueSetup(hueController, "error: no IP address set");
            AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);
            request->send(response);
        } else {
            hueController->flagUserIdSetup();

            AsyncWebServerResponse* response = request->beginResponse(200, "text/html", wait_and_redirect_html);
            request->send(response);
        }
    });

    _server.on("/set_item.html", HTTP_GET, [=](AsyncWebServerRequest* request) {
        std::string banner = "";

        if (request->hasParam(QUERY_ID)) {
            banner = "Item updated!";
            hueController->setItem({request->getParam(QUERY_ID)->value().c_str(), request->hasParam(QUERY_IS_LIGHT)});
        } else {
            banner = "Bad request!";
        }

        renderHueSetup(hueController, banner.c_str());
        AsyncWebServerResponse* response = request->beginResponse(200, "text/html", response_buffer);

        request->send(response);
    });

    _server.onNotFound(onUnhandledRequest);

    _server.begin();
}

void EHS::ConfigurationServer::stop() { _server.end(); }
