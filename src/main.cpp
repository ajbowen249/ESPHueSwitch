#include <ESP8266WiFi.h>

// initial sketch based on examples from the docs
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-examples.html

void setup() {
    Serial.begin(115200);
    delay(5000);
    Serial.println();

    Serial.print("Setting soft-AP ... ");
    boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP");

    if (result) {
        Serial.println("Ready");
    } else {
        Serial.println("Failed!");
    }
}

void loop() {
    Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
    delay(3000);
}
