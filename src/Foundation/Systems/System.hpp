#pragma once

#include <string>
#include <string_view>

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Universe;

struct System {
  System(Universe* u, std::string_view name)
    : universe { u }
    , name { name }
  { }

  virtual ~System() = default;

  virtual bool filter(const Connection& edge) const = 0;
  virtual void swap(Connection& edge) = 0;

  virtual void update();

  Universe* universe;
  std::string name;
};
