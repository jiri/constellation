#pragma once

#include <optional>
#include <unordered_map>

#include <glm/glm.hpp>

#include <Foundation/Systems/System.hpp>

class VideoSystem : public System {
  using Buffer = std::optional<glm::vec3>;
public:
  using System::System;

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void send(Port* port, const glm::vec3& v);
  std::optional<glm::vec3> receive(Port* port);

  std::unordered_map<Port*, Buffer> buffers;
};
