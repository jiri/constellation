#pragma once

#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

#include <Foundation/Systems/System.hpp>

namespace Text {
  struct Buffer {
    std::queue<std::string> messages;
  };

  struct System : public ::System {
    using ::System::System;

    bool filter(const Connection& edge) const override;
    void swap(Connection& edge) override;

    void send(Wiring::Port* port, const std::string& m);
    std::optional<std::string> receive(Wiring::Port* port);

    std::unordered_map<Wiring::Port*, Text::Buffer> buffers;
  };
};
