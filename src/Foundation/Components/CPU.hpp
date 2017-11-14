#pragma once

#include <stack>
#include <sstream>
#include <thread>

#include <fmt/format.h>
#include <Foundation/Wiring.hpp>
#include <Foundation/Component.hpp>

struct CPU : public Component {
  CPU()
    : Component()
    , inPort(this, { { false, 0.0f }, { false, 0.0f }, { true } })
    , outPort(this, { { false, 0.0f }, { false, 0.0f }, { true } })
  { }

  ~CPU() {
    shouldRun = false;
    if (evalThread.joinable()) {
      evalThread.join();
    }
  }

  int pop();
  void eval(std::string program);
  void run(const std::string& program);

  void update() override;
  void render() override;

  std::stack<int> stack;
  std::thread evalThread;
  Wiring::Port inPort;
  Wiring::Port outPort;

  std::atomic_bool shouldRun = false;
};
