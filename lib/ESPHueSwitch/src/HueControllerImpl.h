#pragma once

#include "IHueController.h"

namespace EHS {
class HueControllerImpl : public IHueController {
  public:
    HueControllerImpl();

    virtual const std::string& getIP() const override;
    virtual void setIP(const std::string& newIP) override;

    virtual const HueItemId& getItem() const override;
    virtual void setItem(const HueItemId& newItem) override;

    virtual const std::string& getUserId() const override;
    virtual bool setUpUserId() override;

  private:
    std::string _ip;
    HueItemId _item;
    std::string _userId;
};
} // namespace EHS
