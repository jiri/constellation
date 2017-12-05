#pragma once

#include <Foundation/Infrastructure/Infrastructure.hpp>

class Wireless : public Infrastructure {
public:
  explicit Wireless(Universe* u)
    : Infrastructure { u }
  { }

  void update() override;
};


