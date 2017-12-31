#pragma once

#include <string>

#include <glm/glm.hpp>

namespace gl {
  struct Camera {
    Camera(const glm::vec3& p, const glm::vec3& f);

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up { 0.0f, 1.0f, 0.0f };

    glm::mat4 matrix() const;
  };
}
