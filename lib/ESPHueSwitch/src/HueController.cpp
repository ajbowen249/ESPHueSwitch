#include "HueControllerImpl.h"

#include "ArduinoJSON.h"

#include "HardwareSerial.h"
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"

#define DEFAULT_IP "10.0.0.191"
#define DEFAULT_USERID "Sb-7VtRtDKI4PUdvOO7WtsqR356Xehr3-HZndOK1"
#define DEFAULT_ITEM { "1", false }

char urlBuffer[1024];

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

    if (_ip == "") {
        return false;
    }

    sprintf(urlBuffer, "http://%s/api", _ip.c_str());
    http.begin(client, urlBuffer);

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
    setItemOn(!isItemOn());
}

void EHS::HueControllerImpl::setItemOn(bool isOn) {
    char bodyBuffer[20];
    WiFiClient client;
    HTTPClient http;

    if (_ip == "" || _userId == "" || _item.id == "" || _item.isLight) {
        return;
    }

    sprintf(urlBuffer, "http://%s/api/%s/groups/%s/action", _ip.c_str(), _userId.c_str(), _item.id.c_str());
    http.begin(client, urlBuffer);

    // Eh, we have JSON, but why bother for this?
    sprintf(bodyBuffer, "{\"on\":%s}", isOn ? "true" : "false");
    http.PUT(bodyBuffer);
}

bool EHS::HueControllerImpl::isItemOn() {
    StaticJsonDocument<2048> json;
    WiFiClient client;
    HTTPClient http;

    if (_ip == "" || _userId == "" || _item.id == "" || _item.isLight) {
        return false;
    }

    sprintf(urlBuffer, "http://%s/api/%s/groups/%s", _ip.c_str(), _userId.c_str(), _item.id.c_str());
    http.begin(client, urlBuffer);

    const auto response = http.GET();

    if (response > 0) {
        const auto payload = http.getString();
        const auto deserializeError = deserializeJson(json, payload);
        if (!deserializeError) {
            return json["state"]["any_on"];
        }
    }

    return false;
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
