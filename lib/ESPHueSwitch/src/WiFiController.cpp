#include "ESP8266WiFi.h"
#include "WiFiControllerImpl.h"

#define HOSTNAME "HueSwitch"

EHS::WiFiControllerImpl::WiFiControllerImpl()
    : _status({false, WL_IDLE_STATUS}), _wifiSettings({"", ""}), _useAccessPoint(false), _connectWiFi(false) {}

const EHS::WiFiSettings& EHS::WiFiControllerImpl::getWiFiSettings() const { return _wifiSettings; }

void EHS::WiFiControllerImpl::setWiFiSettings(const EHS::WiFiSettings& settings) {
    _wifiSettings.ssid = settings.ssid;
    _wifiSettings.password = settings.password;
}

const EHS::WiFiStatus& EHS::WiFiControllerImpl::getWiFiStatus() const {
    return _status;
}

bool EHS::WiFiControllerImpl::ConnectWiFi(wl_status_t& result, unsigned long timeoutLength) {
    if (_wifiSettings.ssid != "" && _wifiSettings.password != "") {
        _connectWiFi = true;
        applySettings();

        WiFi.begin(_wifiSettings.ssid.c_str(), _wifiSettings.password.c_str());
        const auto connectResult = WiFi.waitForConnectResult(timeoutLength);
        if (connectResult == -1) {
            _status.lastResult = WL_CONNECT_FAILED;
            _status.isConnected = false;
        } else {
            _status.lastResult = (wl_status_t)connectResult;
            _status.isConnected = connectResult == WL_CONNECTED;
        }
    } else {
        _status.lastResult = WL_NO_SSID_AVAIL;
        _status.isConnected = false;
    }

    result = _status.lastResult;
    return _status.isConnected;
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
