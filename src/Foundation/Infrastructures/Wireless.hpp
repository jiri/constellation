#pragma once

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Antenna : public Port {
public:
  Antenna(Capabilities c, float r)
    : Port { c }
    , radius { r }
  { }

  float radius = 50.0f;
};

class Wireless : public Infrastructure {
public:
  explicit Wireless(Universe* u)
    : Infrastructure { u }
  { }

  void update() override;
};


