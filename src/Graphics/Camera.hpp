#pragma once

#include <string>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

namespace gl {
  class CameraControls {
    friend class Camera;
  private:
    virtual void processEvent(SDL_Event& e) = 0;
    virtual void update(glm::vec3& position, glm::vec3& front) = 0;
  };

  class Camera {
  public:
    Camera(glm::vec3 p, glm::vec3 f, std::unique_ptr<CameraControls> c = nullptr);
    explicit Camera(std::unique_ptr<CameraControls> c);

    void processEvent(SDL_Event& e);
    void update();

    glm::mat4 matrix() const;

  private:
    std::unique_ptr<CameraControls> controls = nullptr;

    glm::vec3 position { 0.0f };
    glm::vec3 front { 0.0f };
    glm::vec3 up { 0.0f, 1.0f, 0.0f };
  };
}
