#pragma once

#include <Foundation/Infrastructures/Infrastructure.hpp>

//class Antenna : public Port {
//
//};

class Wireless : public Infrastructure {
public:
  explicit Wireless(Universe* u)
    : Infrastructure { u }
  { }

  void update() override;
};


