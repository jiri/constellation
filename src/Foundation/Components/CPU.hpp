#pragma once

#include <stack>
#include <sstream>
#include <thread>

#include <fmt/format.h>

#include <Foundation/Infrastructure/Wiring.hpp>
#include <Foundation/Components/Component.hpp>

class Universe;

struct CPU : public Component {
  using ByteCode = std::vector<uint8_t>;

  enum State : uint8_t {
    NORMAL = 0x00,
    HALTED,
    ILLEGAL,
    AWAITING_INPUT,
  };

  CPU(Universe* u)
    : Component(u)
  {
    in.component = this;
    out.component = this;
  }

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

  std::string name() const override {
    return "cpu";
  }

  std::vector<std::pair<std::string, Wiring::Port*>> ports() override {
    return {
        { "in", &in },
        { "out", &out },
    };
  }

  std::string defaultPort() const override {
    return "out";
  }

  State state;
  std::stack<int16_t> stack;

  std::atomic_bool shouldRun = false;
  std::thread evalThread;

  Wiring::Port in = Capabilities {
      .picture = { false, 0.0f },
      .energy = { false, 0.0f },
      .text = { true },
  };

  Wiring::Port out = Capabilities {
    .picture = { false, 0.0f },
    .energy = { false, 0.0f },
    .text = { true },
  };
};
