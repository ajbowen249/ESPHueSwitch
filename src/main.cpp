#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include "ESPHueSwitch.h"

#define DEFAULT_WIFI_SSID "Hanshotfirst (2G)"
#define DEFAULT_WIFI_PASSWORD "12Parsecs"

EHS::SupportObjectBundleImpl supportObjects;
EHS::ConfigurationServer configurationServer(&supportObjects);

void setup() {
    pinMode(2, OUTPUT);
    pinMode(0, INPUT_PULLUP);
    digitalWrite(2, LOW);

    Serial.begin(115200);

    delay(1000);
    Serial.println("start delay...");

    for (int i = 0; i < 10; i++) {
        digitalWrite(2, HIGH);
        delay(250);
        digitalWrite(2, LOW);
        delay(250);
    }

    Serial.println("start delay complete");
    Serial.println("about to init controller");

    const auto wifiController = supportObjects.getWiFiController();

    wifiController->setWiFiSettings({
        DEFAULT_WIFI_SSID,
        DEFAULT_WIFI_PASSWORD,
    });

    Serial.println("set up wifi settings");

    wl_status_t result;
    if (!wifiController->connectWiFi(result, 60000UL)) {
        Serial.printf("WiFi Failed!\n");
    } else {
        Serial.println("connected wifi");
    }

    Serial.println("about to start AP...");
    wifiController->startAccessPoint();
    Serial.println("started access point");

    configurationServer.start();
    Serial.println("started server");

    digitalWrite(2, HIGH);
    delay(1000);
    digitalWrite(2, LOW);
}

void loop() {
    supportObjects.getWiFiController()->loop();
    supportObjects.getHueController()->loop();
    yield();
    digitalWrite(2, !digitalRead(0));
}
