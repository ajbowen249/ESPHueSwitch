#include "SupportObjectBundleImpl.h"

EHS::SupportObjectBundleImpl::SupportObjectBundleImpl() : _wifiController() {}

EHS::IWiFiController* EHS::SupportObjectBundleImpl::getWiFiController() {
    return &_wifiController;
}
