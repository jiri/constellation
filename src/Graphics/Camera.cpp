#include <Graphics/Camera.hpp>

#include <glm/gtx/transform.hpp>

#include <SDL2/SDL.h>

gl::Camera::Camera(glm::vec3 p, glm::vec3 f, std::unique_ptr<gl::CameraControls> c)
  : position { p }
  , front { f }
  , controls { std::move(c) }
{
  this->update();
}

gl::Camera::Camera(std::unique_ptr<gl::CameraControls> c)
  : controls { std::move(c) }
{
  this->update();
}

glm::mat4 gl::Camera::matrix() const {
  SDL_Window* window = SDL_GL_GetCurrentWindow();
  int w = 0;
  int h = 0;
  SDL_GetWindowSize(window, &w, &h);

  return glm::perspective(glm::radians(75.0f), float(w) / float(h), 0.01f, 100.0f)
         * glm::lookAt(position, position + front, up);
}

void gl::Camera::processEvent(SDL_Event& e) {
  if (this->controls) {
    this->controls->processEvent(e);
  }
}

void gl::Camera::update() {
  if (this->controls) {
    this->controls->update(this->position, this->front);
  }
}
