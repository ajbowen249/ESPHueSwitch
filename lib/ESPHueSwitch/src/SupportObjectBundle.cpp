#include "SupportObjectBundleImpl.h"

EHS::SupportObjectBundleImpl::SupportObjectBundleImpl() : _wifiController(), _hueController() {}

EHS::IWiFiController* EHS::SupportObjectBundleImpl::getWiFiController() {
    return &_wifiController;
}

EHS::IHueController* EHS::SupportObjectBundleImpl::getHueController() {
    return &_hueController;
}
