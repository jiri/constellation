#pragma once

#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

#include <Foundation/Systems/System.hpp>

struct TextSystem : public System {
  struct Buffer {
    std::queue<std::string> messages;
  };

  using System::System;

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void send(Port* port, const std::string& m);
  std::optional<std::string> receive(Port* port);

  std::unordered_map<Port*, Buffer> sendBuffers;
  std::unordered_map<Port*, Buffer> recvBuffers;
};
