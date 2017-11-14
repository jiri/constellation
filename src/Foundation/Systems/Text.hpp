#pragma once

#include <optional>
#include <queue>
#include <string>

namespace Text {
  struct Capability {
    bool enabled = false;

    static Capability combine(const Capability& a, const Capability& b);
  };

  struct Buffer {
    std::queue<std::string> messages;

    void send(const std::string& m);
    std::optional<std::string> receive();
  };
};
