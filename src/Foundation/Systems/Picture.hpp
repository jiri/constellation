#pragma once

#include <optional>
#include <unordered_map>

#include <glm/glm.hpp>

#include <Foundation/Systems/System.hpp>

namespace Picture {
  struct Buffer {
    std::optional<glm::vec3> pictureData;
  };

  struct System : public ::System {
    using ::System::System;

    bool filter(const Connection& edge) const override;
    void swap(Connection& edge) override;

    void send(Port* port, const glm::vec3& v);
    std::optional<glm::vec3> receive(Port* port);

    std::unordered_map<Port*, Picture::Buffer> buffers;
  };
};
