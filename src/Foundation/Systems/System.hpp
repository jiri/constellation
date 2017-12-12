#pragma once

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Universe;

struct System {
  System(Universe* u)
    : universe { u }
  { }

  virtual ~System() = default;

  virtual bool filter(const Connection& edge) const = 0;
  virtual void swap(Connection& edge) = 0;

  virtual void update();

  Universe* universe;
};
