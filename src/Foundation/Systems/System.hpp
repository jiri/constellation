#pragma once

#include <Foundation/Infrastructure/Wiring.hpp>

class Universe;

struct System {
  System(Universe* u)
    : universe { u }
  { }

  virtual ~System() = default;

  virtual bool filter(const Wiring::Edge& edge) const = 0;
  virtual void swap(Wiring::Edge& edge) = 0;

  void update();

  Universe* universe;
};
