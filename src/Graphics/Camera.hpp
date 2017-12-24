#pragma once

#include <string>

#include <glm/glm.hpp>

struct GCamera {
  GCamera(std::string name, const glm::vec3& p, const glm::vec3& f);

  std::string name;

  glm::vec3 position;
  glm::vec3 front;

  glm::vec3 up { 0.0f, 1.0f, 0.0f };

  glm::mat4 matrix() const;
};
