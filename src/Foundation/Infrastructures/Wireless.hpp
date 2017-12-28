#pragma once

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Antenna : public Endpoint {
public:
  Antenna(float r, float f, Capabilities c)
    : Endpoint { c }
    , radius { r }
    , frequency { f }
  { }

  float radius;
  float frequency;
};

class Wireless : public Infrastructure {
public:
  using Infrastructure::Infrastructure;

  void update() override;
};


