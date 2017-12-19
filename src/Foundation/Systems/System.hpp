#pragma once

#include <string>
#include <string_view>
#include <chrono>

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Universe;

struct System {
  System(Universe* u, std::string_view name, uint32_t ups = 20)
    : universe { u }
    , name { name }
    , ups { ups }
  { }

  virtual ~System() = default;

  virtual bool filter(const Connection& edge) const = 0;
  virtual void swap(Connection& edge) = 0;

  void timePassed(std::chrono::system_clock::duration d);
  virtual void update();

  virtual void UI();

  Universe* universe = nullptr;

  std::string name;
  bool active = true;

  uint32_t ups;
  float actualUps = 0.0f;
  std::chrono::system_clock::duration deltaCounter { 0 };
  std::chrono::system_clock::duration upsTimeCounter { 0 };
  uint32_t upsCounter = 0;
};
