#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include "ESPHueSwitch.h"

#define DEFAULT_WIFI_SSID "Hanshotfirst (2G)"
#define DEFAULT_WIFI_PASSWORD "12Parsecs"

EHS::SupportObjectBundleImpl supportObjects;
EHS::ConfigurationServer configurationServer(&supportObjects);

void setup() {
    Serial.begin(115200);

    delay(1000);
    Serial.println("start delay...");
    delay(5000);
    Serial.println("start delay complete");
    Serial.println("about to init controller");

    const auto wifiController = supportObjects.getWiFiController();

    wifiController->setWiFiSettings({
        DEFAULT_WIFI_SSID,
        DEFAULT_WIFI_PASSWORD,
    });

    Serial.println("set up wifi settings");

    wl_status_t result;
    if (!wifiController->ConnectWiFi(result, 60000UL)) {
        Serial.printf("WiFi Failed!\n");
    } else {
        Serial.println("connected wifi");
    }

    configurationServer.start();
    Serial.println("started server");
}

void loop() {
    supportObjects.getWiFiController()->loop();
    yield();
}
