#pragma once

#include <Foundation/Infrastructure/Wiring.hpp>

class Universe;

struct System {
  System(Universe* u)
    : universe { u }
  { }

  virtual ~System() = default;

  virtual bool filter(const Wiring::Connection& edge) const = 0;
  virtual void swap(Wiring::Connection& edge) = 0;

  void update();

  Universe* universe;
};
