#include "HueControllerImpl.h"

#include "ArduinoJSON.h"

#include "HardwareSerial.h"
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"

#define DEFAULT_IP "10.0.0.191"
#define DEFAULT_USERID "Sb-7VtRtDKI4PUdvOO7WtsqR356Xehr3-HZndOK1"
#define DEFAULT_ITEM { "1", false }

char urlBuffer[1024];

const char* urlTemplate = "http://%s/api/%s/%s";

EHS::HueControllerImpl::HueControllerImpl() : _ip(DEFAULT_IP), _item(DEFAULT_ITEM), _userId(DEFAULT_USERID), _shouldSetUpUser(false), _shouldToggle(false) {}

const std::string& EHS::HueControllerImpl::getIP() const {
    return _ip;
}

void EHS::HueControllerImpl::setIP(const std::string& newIP) {
    _ip = newIP;
}

const EHS::HueItemId& EHS::HueControllerImpl::getItem() const {
    return _item;
}

void EHS::HueControllerImpl::setItem(const EHS::HueItemId& newItem) {
    _item = newItem;
}

const std::string& EHS::HueControllerImpl::getUserId() const {
    return _userId;
}

bool EHS::HueControllerImpl::setUpUserId() {
    StaticJsonDocument<1024> json;
    WiFiClient client;
    HTTPClient http;

    Serial.println("setting up user ID");

    if (_ip == "") {
        return false;
    }

    Serial.println("about to write URL");
    sprintf(urlBuffer, "http://%s/api", _ip.c_str());
    Serial.println(urlBuffer);

    Serial.println("about to begin");
    http.begin(client, urlBuffer);
    Serial.println("about to post");

    bool success = false;
    const auto response = http.POST("{\"devicetype\" : \"hue_switch#hue_switch hue_switch\"}");

    if (response > 0) {
        Serial.println("success response");
        const auto payload = http.getString();
        Serial.println(payload);

        const auto deserializeError = deserializeJson(json, payload);
        if (deserializeError) {
            Serial.println("Deserialization error");
            Serial.println(deserializeError.c_str());
            success = false;
        } else {
            // Responses are wrapped in an outer array from Hue, even when it's a single thing
            /*
            [
                {
                    "success": {
                        "username": "the_username"
                    }
                }
            ]

            [
                {
                    "error": {
                        "type": 101,
                        "address": "",
                        "description": "link button not pressed"
                    }
                }
            ]
            */
            const auto responseItem = json[0];
            if (responseItem.containsKey("success")) {
                const char* userId = responseItem["success"]["username"];
                Serial.println(userId);
                _userId = userId;
                success = true;
            } else if (responseItem.containsKey("error")) {
                const char* description = responseItem["error"]["description"];
                Serial.println(description);
                success = false;
            } else {
                Serial.println("unknown error");
                success = false;
            }
        }
    } else {
        Serial.println("error response");
        Serial.println(response);
        success = false;
    }

    http.end();

    return success;
}

void EHS::HueControllerImpl::toggle() {
    Serial.println("Toggle test in hue controller.");
}

void EHS::HueControllerImpl::flagUserIdSetup() {
    _shouldSetUpUser = true;
}

void EHS::HueControllerImpl::flagToggle() {
    _shouldToggle = true;
}

void EHS::HueControllerImpl::loop() {
    if (_shouldSetUpUser) {
        _shouldSetUpUser = false;
        setUpUserId();
    }

    if (_shouldToggle) {
        _shouldToggle = false;
        toggle();
    }
}
