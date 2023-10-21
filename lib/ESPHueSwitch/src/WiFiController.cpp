#include "ESP8266WiFi.h"
#include "WiFiControllerImpl.h"

#define HOSTNAME "HueSwitch"

EHS::WiFiControllerImpl::WiFiControllerImpl() : _wifiSettings({"", ""}), _useAccessPoint(false), _connectWiFi(false) {}

const EHS::WiFiSettings EHS::WiFiControllerImpl::getWiFiSettings() const {
    return _wifiSettings;
}

void EHS::WiFiControllerImpl::setWiFiSettings(const EHS::WiFiSettings& settings) {
    _wifiSettings.ssid = settings.ssid;
    _wifiSettings.password = settings.password;
}

bool EHS::WiFiControllerImpl::ConnectWiFi(wl_status_t& result, unsigned long timeoutLength) {

    if (_wifiSettings.ssid != "" && _wifiSettings.password != "") {
        _connectWiFi = true;
        applySettings();

        WiFi.begin(_wifiSettings.ssid.c_str(), _wifiSettings.password.c_str());
        const auto connectResult = WiFi.waitForConnectResult(timeoutLength);
        if (connectResult == -1) {
            result = WL_CONNECT_FAILED;
            return false;
        }

        result = (wl_status_t)connectResult;
        return result == WL_CONNECTED;
    }

    result = WL_NO_SSID_AVAIL;
    return false;
}

void EHS::WiFiControllerImpl::StartAccessPoint() {}

void EHS::WiFiControllerImpl::applySettings() {
    if (_useAccessPoint && _connectWiFi) {
        WiFi.mode(WIFI_AP_STA);
    } else if (_useAccessPoint) {
        WiFi.mode(WIFI_AP);
    } else if (_connectWiFi) {
        WiFi.mode(WIFI_STA);
    }

    WiFi.hostname(HOSTNAME);
}
