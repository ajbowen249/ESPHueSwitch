#pragma once

#include <string>

namespace EHS {
// Can Id either a light or a group
struct HueItemId {
    std::string id;
    bool isGroup;
};

class IHueController {
  public:
    virtual const std::string& getIP() const;
    virtual void setIP(const std::string& newIP);

    virtual const HueItemId& getItem() const;
    virtual void setItem(const HueItemId& newItem);

    virtual const std::string& getUserId() const;
    virtual bool setUpUserId();
    virtual void flagUserIdSetup();

    virtual void loop();
};
} // namespace EHS
