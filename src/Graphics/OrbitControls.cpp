#include <Graphics/OrbitControls.hpp>

#include <glm/gtc/constants.hpp>

#include <imgui.h>

OrbitControls::OrbitControls(glm::vec3 target, float radius)
  : target { target }
  , radius { radius }
{ }

void OrbitControls::processEvent(SDL_Event &e) {
  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }

  switch (e.type) {
    case SDL_MOUSEMOTION:
      if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        dx += e.motion.xrel;
        dy += e.motion.yrel;
      }
      break;

    case SDL_MOUSEWHEEL:
      dz += e.wheel.y;
      break;

    default:
      break;
  }
}

void OrbitControls::update(glm::vec3& position, glm::vec3& front) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    /* Process input */
    yaw    += 0.005f * dx;
    pitch  -= 0.005f * dy;
    radius += 0.020f * dz;
  }

  /* Set the position */
  auto eps = std::numeric_limits<float>::epsilon();

  pitch = glm::clamp(pitch, -glm::pi<float>() / 2.0f + eps, glm::pi<float>() / 2.0f - eps);
  radius = glm::clamp(radius, 1.0f + eps, 50.0f - eps);

  glm::vec3 direction { cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * sin(-yaw) };
  position = target + radius * direction;
  front = glm::normalize(target - position);

  /* Reset state */
  dx = 0.0f;
  dy = 0.0f;
  dz = 0.0f;
}
