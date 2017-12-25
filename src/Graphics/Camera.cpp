#include <Graphics/Camera.hpp>

#include <glm/gtx/transform.hpp>

#include <SDL2/SDL.h>

GCamera::GCamera(std::string name, const glm::vec3 &p, const glm::vec3 &f)
  : name { std::move(name) }
  , position { p }
  , front { f }
{ }

glm::mat4 GCamera::matrix() const {
  SDL_Window* window = SDL_GL_GetCurrentWindow();
  int w = 0;
  int h = 0;
  SDL_GetWindowSize(window, &w, &h);

  return glm::perspective(glm::radians(75.0f), float(w) / float(h), 0.01f, 100.0f)
         * glm::lookAt(position, position + front, up);
}
