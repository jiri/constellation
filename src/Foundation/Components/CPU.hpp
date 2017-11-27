#pragma once

#include <stack>
#include <sstream>
#include <thread>

#include <fmt/format.h>

#include <Foundation/Wiring.hpp>
#include <Foundation/Component.hpp>

class Universe;

struct CPU : public Component {
  using ByteCode = std::vector<uint8_t>;

  enum State : uint8_t {
    NORMAL = 0x00,
    HALTED,
    ILLEGAL,
  };

  CPU(Universe* u)
    : Component(u)
    , inPort(this, { { false, 0.0f }, { false, 0.0f }, { true } })
    , outPort(this, { { false, 0.0f }, { false, 0.0f }, { true } })
  { }

  ~CPU() override {
    shouldRun = false;
    if (evalThread.joinable()) {
      evalThread.join();
    }
  }

  int16_t pop();
  void push(int16_t x);

  static ByteCode compile(const std::string& program);
  void execute(const ByteCode& code);
  void run(const ByteCode& code);

  void update() override;
  void render() override;

  State state;
  std::stack<int16_t> stack;

  std::atomic_bool shouldRun = false;
  std::thread evalThread;

  Wiring::Port inPort;
  Wiring::Port outPort;
};