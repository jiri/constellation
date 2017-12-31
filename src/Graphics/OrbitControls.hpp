#pragma once

#include <Graphics/Camera.hpp>

class OrbitControls : public gl::CameraControls {
public:
  OrbitControls(glm::vec3 target, float radius);

  void processEvent(SDL_Event& e) override;
  void update(glm::vec3& position, glm::vec3& front) override;

private:
  glm::vec3 target;

  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;

  float radius;

  float pitch = 0.0f;
  float yaw = 0.0f;
};
