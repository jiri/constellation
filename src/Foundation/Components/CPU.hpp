#pragma once

#include <stack>
#include <sstream>
#include <fmt/format.h>

#include <Foundation/Wiring.hpp>
#include <Foundation/Component.hpp>

struct CPU : public Component {
  CPU()
    : Component()
    , port(this, { { false, 0.0f }, { false, 0.0f }, { true } })
  { }

  std::stack<int> stack;

  int pop();

  void run(const std::string& program);

  void update() override;
  void render() override;

  Wiring::Port port;
};
