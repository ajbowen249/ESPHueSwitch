#include "HueControllerImpl.h"

#define DEFAULT_IP "10.0.0.191"
#define DEFAULT_USERID ""

EHS::HueControllerImpl::HueControllerImpl() : _ip(DEFAULT_IP), _item({ "", false }), _userId(DEFAULT_USERID) {}

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
    return false;
}
