#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <Graphics/Camera.hpp>

class OrbitControls {
public:
  OrbitControls(GCamera &c, glm::vec3 target, float radius);

  void processEvent(SDL_Event& e);
  void update();

  glm::vec3 target;

private:
  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;

  float radius;

  GCamera* camera;

  float pitch = 0.0f;
  float yaw = 0.0f;
};
