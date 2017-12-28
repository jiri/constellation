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

  explicit TextSystem(Universe* u)
    : System { u, "Text" }
  { }

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void send(Endpoint* port, const std::string& m);
  std::optional<std::string> receive(Endpoint* port);

  std::unordered_map<Endpoint*, Buffer> sendBuffers;
  std::unordered_map<Endpoint*, Buffer> recvBuffers;
};
