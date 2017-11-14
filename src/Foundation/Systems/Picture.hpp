#pragma once

#include <optional>

#include <glm/glm.hpp>

namespace Picture {
  struct Capability {
    bool enabled = false;
    float errorRate = 0.0f;

    static Capability combine(const Capability& a, const Capability& b);
  };

  struct Buffer {
    std::optional<glm::vec3> pictureData;

    void send(const glm::vec3& v);
    std::optional<glm::vec3> receive();

    void clear();
  };
};
